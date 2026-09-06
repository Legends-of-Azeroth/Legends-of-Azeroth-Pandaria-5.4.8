SET @CGUID := 50000000;
SET @OGUID := 50000000;
SET @NPCTEXTID := 50000000;

DELETE FROM creature_equip_template WHERE (CreatureID=60009 AND ID=1);
INSERT INTO creature_equip_template (CreatureID, ID, ItemID1, AppearanceModID1, ItemVisual1, ItemID2, AppearanceModID2, ItemVisual2, ItemID3, AppearanceModID3, ItemVisual3, VerifiedBuild) VALUES
(60009, 1, 82771, 0, 0, 0, 0, 0, 0, 0, 0, 46366); -- Feng the Accursed

DELETE FROM areatrigger_template WHERE (IsServerSide=0 AND Id IN (3464,3616,3101,2978,2891,3541));
INSERT INTO areatrigger_template (Id, IsServerSide, Type, Flags, Data0, Data1, Data2, Data3, Data4, Data5, Data6, Data7, VerifiedBuild) VALUES
(3464, 0, 0, 4, 6.5, 6.5, 0, 0, 0, 0, 0, 0, 46366),
(3616, 0, 0, 4, 2.5, 2.5, 0, 0, 0, 0, 0, 0, 46366),
(3101, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 46366),
(2978, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 46366),
(2891, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 46366),
(3541, 0, 0, 0, 3.5, 3.5, 0, 0, 0, 0, 0, 0, 46366);

UPDATE areatrigger_template SET Flags=2 WHERE (Id=9228 AND IsServerSide=0);

UPDATE gameobject_template_addon SET faction=114 WHERE entry=211603; -- Terracotta Army Door
UPDATE gameobject_template_addon SET flags=16 WHERE entry=211584; -- Ancient Control Console
UPDATE gameobject_template_addon SET faction=94, flags=2113536 WHERE entry=214383; -- Cache of Pure Energy
UPDATE gameobject_template_addon SET flags=16 WHERE entry=211650; -- Celestial Control Console
UPDATE gameobject_template_addon SET flags=16 WHERE entry=214412; -- Celestial Door
UPDATE gameobject_template_addon SET faction=1375 WHERE entry=215114; -- Wind Wall
UPDATE gameobject_template_addon SET faction=1375 WHERE entry=213933; -- Mogu_Raised_Gate01
UPDATE gameobject_template_addon SET flags=1 WHERE entry=211628; -- Shroud of Reversal
UPDATE gameobject_template_addon SET flags=1 WHERE entry=211626; -- Nullification Barrier
UPDATE gameobject_template_addon SET faction=114 WHERE entry=211311; -- Quilin Rubble
UPDATE gameobject_template_addon SET faction=114, flags=16 WHERE entry=214497; -- Stone Guard Entrance Door
UPDATE gameobject_template_addon SET flags=48 WHERE entry=213506; -- Titan Energy Discs
UPDATE gameobject_template_addon SET flags=48 WHERE entry=213373; -- Mysterious Wall
UPDATE gameobject_template_addon SET flags=48 WHERE entry IN (213950, 213951, 213948, 213949, 213946, 213947, 213944, 213945, 213942, 213943, 213940, 213941, 213938, 213939, 213937, 213954, 213955, 213952); -- Mogu Energy Rune
UPDATE gameobject_template_addon SET flags=48 WHERE entry IN (213528, 213529, 213527); -- Titan Energy Ring
UPDATE gameobject_template_addon SET flags=32 WHERE entry=213953; -- Mogu Energy Rune
UPDATE gameobject_template_addon SET faction=1375, flags=32 WHERE entry=213246; -- Doodad_Mogu_Statue_MoguShanVaults001
UPDATE gameobject_template_addon SET faction=1375, flags=32 WHERE entry=213247; -- Doodad_Mogu_Statue_MoguShanVaults004
UPDATE gameobject_template_addon SET faction=114, flags=48 WHERE entry IN (214526, 214696); -- Ancient Mogu Door
UPDATE gameobject_template_addon SET faction=1375, flags=32 WHERE entry IN (213244, 213258); -- Ancient Mogu Door
UPDATE gameobject_template_addon SET faction=1375, flags=32 WHERE entry=213245; -- Doodad_Mogu_Statue_MoguShanVaults002
UPDATE gameobject_template_addon SET faction=1375, flags=32 WHERE entry=213248; -- Doodad_Mogu_Statue_MoguShanVaults003
UPDATE gameobject_template_addon SET faction=114, flags=32 WHERE entry=214452; -- Feng's Door
UPDATE gameobject_template_addon SET faction=1375, flags=48 WHERE entry=213285; -- Ancient Mogu Door
UPDATE gameobject_template_addon SET faction=114, flags=32 WHERE entry=210868; -- Outer Doors
UPDATE gameobject_template_addon SET faction=114, flags=32 WHERE entry=211693; -- Inner Doors
UPDATE gameobject_template_addon SET faction=35, flags=8192 WHERE entry=214441; -- Instance Portal (Raid 4 Difficulties)

DELETE FROM creature WHERE guid BETWEEN @CGUID+0 AND @CGUID+411;
INSERT INTO creature (guid, id, map, zoneId, areaId, spawnDifficulties, PhaseId, PhaseGroup, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, currentwaypoint, curhealth, curmana, MovementType, npcflag, unit_flags, dynamicflags, VerifiedBuild) VALUES
(@CGUID+0, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3893.181396484375, 1136.10595703125, 481.129364013671875, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+1, 64183, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.366943359375, 1205.4571533203125, 466.23101806640625, 1.583484768867492675, 7200, 0, 0, 45960, 0, 0, 0, 0, 0, 46366), -- Enormous Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field)
(@CGUID+2, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3908.986083984375, 1132.1771240234375, 481.133575439453125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+3, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3889.677001953125, 1132.1875, 481.133636474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+4, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3899.475830078125, 1045.201416015625, 485.833343505859375, 1.548666000366210937, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+5, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3880.8212890625, 1197.232666015625, 466.261505126953125, 4.694097518920898437, 7200, 10, 0, 11490, 0, 1, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form) (possible waypoints or random movement)
(@CGUID+6, 64825, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3903.447998046875, 1039.1475830078125, 485.829498291015625, 2.615472316741943359, 7200, 10, 0, 34470, 0, 1, 0, 0, 0, 46366), -- Leng Windstaff (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+7, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3868.9375, 1197.40283203125, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+8, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3877.038330078125, 1205.420166015625, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+9, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3920.4609375, 1128.5694580078125, 481.135498046875, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+10, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3916.612060546875, 1128.5191650390625, 481.1329345703125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+11, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3913.489501953125, 1201.4879150390625, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+12, 36737, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3902.756103515625, 1040.76220703125, 485.829498291015625, 0, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- Invisible Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+13, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3884.915771484375, 1201.6041259765625, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+14, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3885.88623046875, 1132.201416015625, 481.136199951171875, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+15, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3881.888916015625, 1128.5069580078125, 481.132659912109375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+16, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3905.708251953125, 1136.0572509765625, 481.12939453125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+17, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3912.782958984375, 1132.1632080078125, 481.136138916015625, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+18, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3872.89501953125, 1201.392333984375, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+19, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3917.90283203125, 1197.15283203125, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+20, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3880.848876953125, 1205.5816650390625, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+21, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3873.026123046875, 1209.4791259765625, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+22, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3877.69091796875, 1128.3507080078125, 481.1353759765625, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+23, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.403564453125, 1487.8177490234375, 368.3076171875, 1.819480299949645996, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+24, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3835.315185546875, 1502.10595703125, 398.912109375, 0.826354920864105224, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+25, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3930.099853515625, 1213.701416015625, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+26, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3814.869873046875, 1563.532958984375, 367.726531982421875, 6.079211711883544921, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+27, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3926.041748046875, 1201.5399169921875, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+28, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.439208984375, 1611.7899169921875, 368.3076171875, 4.795518875122070312, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+29, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3814.6494140625, 1537.170166015625, 367.72650146484375, 0.197296559810638427, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+30, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3853.45751953125, 1607.8228759765625, 368.3076171875, 5.137610912322998046, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+31, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3813.387939453125, 1550.4149169921875, 398.333038330078125, 6.279162883758544921, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+32, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3853.1025390625, 1491.9254150390625, 398.912078857421875, 1.144058823585510253, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+33, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3835.51123046875, 1598.2135009765625, 368.307647705078125, 5.454387187957763671, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+34, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.455810546875, 1612.35595703125, 398.91204833984375, 4.794502735137939453, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+35, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3853.053955078125, 1492.015625, 368.3076171875, 1.142781615257263183, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+36, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3917.97216796875, 1205.611083984375, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+37, 64183, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3903.302734375, 1212.6966552734375, 466.231048583984375, 1.569559931755065917, 7200, 0, 0, 45960, 0, 0, 0, 0, 0, 46366), -- Enormous Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field)
(@CGUID+38, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3917.90966796875, 1213.857666015625, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+39, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3921.907958984375, 1205.6041259765625, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+40, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3835.689208984375, 1598.0728759765625, 398.912109375, 5.453831195831298828, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+41, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3880.919189453125, 1213.8038330078125, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+42, 60089, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3899.203125, 1234.3697509765625, 466.261474609375, 4.709531307220458984, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Stone Guard Controller (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field)
(@CGUID+43, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3913.51904296875, 1209.6649169921875, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+44, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3820.60595703125, 1581.48095703125, 368.327789306640625, 5.794788360595703125, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+45, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.364501953125, 1487.66845703125, 368.3076171875, 1.487590670585632324, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+46, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3814.522705078125, 1537.2430419921875, 398.3341064453125, 0.195843532681465148, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+47, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3884.82373046875, 1209.654541015625, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+48, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3820.20751953125, 1518.8629150390625, 368.327789306640625, 0.484999984502792358, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+49, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.405517578125, 1488.013916015625, 398.91204833984375, 1.48778235912322998, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+50, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3926.056396484375, 1209.5260009765625, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+51, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3813.39404296875, 1550.3680419921875, 367.7254638671875, 6.279870033264160156, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+52, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3930.040771484375, 1197.6180419921875, 466.261474609375, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+53, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.210205078125, 1612.170166015625, 398.912017822265625, 4.465447902679443359, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+54, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.30908203125, 1612.1927490234375, 368.307586669921875, 4.464034557342529296, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+55, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3835.225830078125, 1501.8629150390625, 368.307647705078125, 0.827863872051239013, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+56, 60576, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3869.014892578125, 1213.7725830078125, 466.261505126953125, 4.694097518920898437, 7200, 0, 0, 11490, 0, 0, 0, 0, 0, 46366), -- Stone Quilen (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126816 - Dampening Field, 117838 - Stone Form)
(@CGUID+57, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3814.838623046875, 1563.4166259765625, 398.334075927734375, 6.081032276153564453, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+58, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3853.236083984375, 1608.375, 398.912078857421875, 5.137202739715576171, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+59, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.317626953125, 1487.5694580078125, 398.912017822265625, 1.817242026329040527, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+60, 60043, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3878.92529296875, 1258.4879150390625, 466.36334228515625, 4.694972991943359375, 7200, 0, 0, 738000, 100, 0, 0, 0, 0, 46366), -- Jade Guardian (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit)
(@CGUID+61, 60047, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3928.005126953125, 1246.34375, 466.36334228515625, 4.711470603942871093, 7200, 0, 0, 738000, 100, 0, 0, 0, 0, 46366), -- Amethyst Guardian (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit)
(@CGUID+62, 59915, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3919.89404296875, 1258.5867919921875, 466.36334228515625, 4.669905662536621093, 7200, 0, 0, 738000, 100, 0, 0, 0, 0, 46366), -- Jasper Guardian (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit)
(@CGUID+63, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.030517578125, 1483.623291015625, 398.911956787109375, 1.488137125968933105, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+64, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.553955078125, 1483.5035400390625, 398.9119873046875, 1.591090679168701171, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+65, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.310791015625, 1483.420166015625, 368.307586669921875, 1.558365225791931152, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+66, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3851.71435546875, 1487.3629150390625, 398.912017822265625, 1.286050081253051757, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+67, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3894.74560546875, 1485.93408203125, 368.30755615234375, 1.488137125968933105, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+68, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3851.33935546875, 1486.515625, 368.3076171875, 1.230633974075317382, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+69, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3832.239501953125, 1498.482666015625, 398.912017822265625, 0.810256123542785644, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+70, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3832.317626953125, 1498.5347900390625, 368.3076171875, 0.842961847782135009, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+71, 59481, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4014.510498046875, 1297.3367919921875, 477.870849609375, 0.986037492752075195, 7200, 0, 0, 168939, 17038, 0, 0, 0, 0, 46366), -- World Trigger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+73, 61334, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3907.9619140625, 1337.4757080078125, 466.686279296875, 2.677329063415527343, 7200, 0, 0, 55550, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120661 - Bronze, 120613 - Pose)
(@CGUID+74, 61334, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3892.09716796875, 1341.484375, 466.738067626953125, 6.013867378234863281, 7200, 0, 0, 55550, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120661 - Bronze, 120613 - Pose)
(@CGUID+75, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3816.4140625, 1516.7864990234375, 368.327728271484375, 0.615614175796508789, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+76, 62004, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3900.33251953125, 1340.3785400390625, 474.136077880859375, 0, 7200, 0, 0, 19998, 7469, 0, 0, 0, 0, 46366), -- Ghost Essence (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120681 - Ghost Essence)
(@CGUID+77, 61989, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3989.328125, 1330.7447509765625, 462.025177001953125, 3.12713170051574707, 7200, 0, 0, 33330, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120663 - Stone, 120650 - Pose)
(@CGUID+78, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3811.624267578125, 1536.6614990234375, 398.334014892578125, 0.749377310276031494, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+79, 61989, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3989.327392578125, 1336.2222900390625, 462.0556640625, 3.124518632888793945, 7200, 0, 0, 33330, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120663 - Stone, 120651 - Pose)
(@CGUID+80, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3811.987060546875, 1536.68408203125, 367.726470947265625, 0.225580394268035888, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+81, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3810.897705078125, 1550.59375, 398.3330078125, 6.225369930267333984, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+82, 61989, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3989.474853515625, 1341.720458984375, 462.060821533203125, 3.12713170051574707, 7200, 0, 0, 33330, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120663 - Stone, 120650 - Pose)
(@CGUID+83, 61989, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3989.38623046875, 1352.75, 462.0250244140625, 3.12713170051574707, 7200, 0, 0, 33330, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120663 - Stone, 120650 - Pose)
(@CGUID+84, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3811, 1550.392333984375, 367.725372314453125, 6.252098560333251953, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+85, 61989, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3989.5390625, 1347.2447509765625, 462.05828857421875, 3.135972261428833007, 7200, 0, 0, 33330, 1067, 0, 0, 0, 0, 46366), -- Cursed Mogu Sculpture (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 120663 - Stone, 120651 - Pose)
(@CGUID+86, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3812.20654296875, 1564.123291015625, 398.334014892578125, 5.772309303283691406, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+87, 59481, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4067.01904296875, 1292.796875, 477.093658447265625, 2.303777694702148437, 7200, 0, 0, 168939, 17038, 0, 0, 0, 0, 46366), -- World Trigger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+88, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3812.33154296875, 1564.236083984375, 367.726470947265625, 6.039221763610839843, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+89, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3816.6572265625, 1583.9149169921875, 368.3277587890625, 5.709687232971191406, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+90, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.608642578125, 1617.904541015625, 398.9119873046875, 4.754971981048583984, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+91, 59481, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4015.4072265625, 1388.6024169921875, 477.997344970703125, 5.27372598648071289, 7200, 0, 0, 168939, 17038, 0, 0, 0, 0, 46366), -- World Trigger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+92, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3850.6962890625, 1613.53125, 368.307586669921875, 5.073643207550048828, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+93, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.53125, 1617.470458984375, 398.911956787109375, 4.826898097991943359, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+94, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3831.7890625, 1602.7257080078125, 398.91204833984375, 5.416833400726318359, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+95, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3831.357666015625, 1602.69970703125, 368.3076171875, 5.423614025115966796, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+96, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3895.79248046875, 1618.0416259765625, 368.307525634765625, 4.689435005187988281, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+97, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3850.744873046875, 1613.93408203125, 398.912017822265625, 5.057805538177490234, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+98, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3874.319580078125, 1618.0069580078125, 368.30755615234375, 4.717673778533935546, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+99, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3899.475830078125, 1045.201416015625, 485.833343505859375, 1.548666000366210937, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+100, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3889.59375, 1303.5572509765625, 470.974029541015625, 6.263474941253662109, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+101, 59481, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4069.088623046875, 1389.189208984375, 477.6573486328125, 4.194894790649414062, 7200, 0, 0, 168939, 17038, 0, 0, 0, 0, 46366), -- World Trigger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+102, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.2666015625, 1484.0538330078125, 459.92388916015625, 1.584045052528381347, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+103, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.975830078125, 1488.65283203125, 459.9239501953125, 1.227036595344543457, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+104, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3874.697998046875, 1485.1007080078125, 490.000244140625, 1.599774122238159179, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+105, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3874.223876953125, 1484.84033203125, 429.31951904296875, 1.579233169555664062, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+106, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.262939453125, 1485.0677490234375, 490.000244140625, 1.584569692611694335, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+107, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.48095703125, 1484.38720703125, 429.319488525390625, 1.595164179801940917, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+108, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3874.392333984375, 1484.8992919921875, 520.6046142578125, 1.586458325386047363, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+109, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3874.864501953125, 1484.85595703125, 459.9239501953125, 1.60658121109008789, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+110, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.23779296875, 1484.0538330078125, 520.6046142578125, 1.582613825798034667, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+111, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.296875, 1488.2135009765625, 490.000274658203125, 1.202489614486694335, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+112, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.888916015625, 1487.592041015625, 520.60467529296875, 1.240563273429870605, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+113, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.65625, 1489.2691650390625, 429.319580078125, 1.20012056827545166, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+114, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3833.061767578125, 1499.373291015625, 459.92401123046875, 0.966363131999969482, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+115, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3833.458251953125, 1499.295166015625, 490.000274658203125, 0.984136581420898437, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+116, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3833.552001953125, 1499.7447509765625, 429.319580078125, 0.975777983665466308, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+117, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3832.8447265625, 1498.748291015625, 520.60467529296875, 0.974517226219177246, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+118, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3817.940185546875, 1517.0052490234375, 429.322540283203125, 0.527526378631591796, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+119, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3816.6416015625, 1516.6024169921875, 490.003265380859375, 0.509466469287872314, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+120, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3816.73876953125, 1516.5972900390625, 459.927001953125, 0.512550890445709228, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+121, 61124, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4022.186767578125, 1362.52783203125, 466.388641357421875, 5.452180862426757812, 7200, 0, 0, 9840, 0, 0, 0, 0, 0, 46366), -- Phase Controller (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118486 - Phase Transition (Staff))
(@CGUID+122, 61124, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4022.4775390625, 1320.8194580078125, 466.399688720703125, 0.777024626731872558, 7200, 0, 0, 9840, 0, 0, 0, 0, 0, 46366), -- Phase Controller (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118485 - Phase Transition (Spear))
(@CGUID+123, 61124, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4061.86376953125, 1320.8228759765625, 466.388641357421875, 2.194269418716430664, 7200, 0, 0, 9840, 0, 0, 0, 0, 0, 46366), -- Phase Controller (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 105032 - Phase Transition (Fist))
(@CGUID+124, 60009, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4041.981689453125, 1341.859375, 466.388427734375, 3.140162944793701171, 7200, 0, 0, 861000, 118900, 0, 0, 0, 0, 46366), -- Feng the Accursed (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 124252 - Unarmed)
(@CGUID+125, 60512, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4214.40966796875, 1435.078125, 454.417266845703125, 0, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Spirit Totem (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+126, 60558, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4215.48193359375, 1415.9288330078125, 454.417266845703125, 0.570452272891998291, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+127, 60385, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4220.87158203125, 1416.623291015625, 454.417266845703125, 5.108249664306640625, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari War Wyvern (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+128, 60558, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4217.0712890625, 1433.1007080078125, 454.417266845703125, 5.571400642395019531, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+129, 60512, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4213.80126953125, 1413.251708984375, 454.417266845703125, 0, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Spirit Totem (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+130, 60143, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4227.05126953125, 1423.4930419921875, 454.417266845703125, 0, 7200, 0, 0, 664200, 0, 0, 0, 0, 0, 46366), -- Gara'jal the Spiritbinder (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 115964 - Mighty Mojo)
(@CGUID+131, 60559, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4231.9443359375, 1420.626708984375, 454.417266845703125, 2.576763868331909179, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+132, 60384, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4242.54052734375, 1333.9326171875, 474.834808349609375, 1.733417153358459472, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Zandalari Pterror Wing (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+133, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4120.736328125, 1356.6771240234375, 462.032958984375, 0.132113084197044372, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+134, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4180.6943359375, 1336.9427490234375, 462.045166015625, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116545 - Troll Explosives)
(@CGUID+135, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4183.41162109375, 1357.453125, 462.041839599609375, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+136, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4183.01904296875, 1345.6319580078125, 462.045166015625, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+137, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4196.86962890625, 1355.3211669921875, 462.04168701171875, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116545 - Troll Explosives)
(@CGUID+138, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4204.5537109375, 1330.329833984375, 462.0469970703125, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+139, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4192.08154296875, 1325.1163330078125, 462.036529541015625, 3.671204805374145507, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116545 - Troll Explosives)
(@CGUID+140, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4194.4228515625, 1350.171875, 462.045166015625, 2.557350635528564453, 7200, 10, 0, 23780, 0, 1, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%) (possible waypoints or random movement)
(@CGUID+141, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4200.1083984375, 1334.717041015625, 462.046142578125, 3.027443170547485351, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+142, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4192.91162109375, 1335.8629150390625, 462.045166015625, 2.516661643981933593, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+143, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4199.40966796875, 1350.27783203125, 462.045654296875, 3.919560670852661132, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+144, 60375, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4177.26904296875, 1339.0294189453125, 461.979705810546875, 2.629953861236572265, 7200, 0, 0, 28536, 0, 0, 0, 0, 0, 46366), -- Zandalari Skullcharger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll, 120379 - Mod Scale 95-100%)
(@CGUID+145, 60375, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4177.29248046875, 1345.9832763671875, 461.982513427734375, 3.518621206283569335, 7200, 0, 0, 28536, 0, 0, 0, 0, 0, 46366), -- Zandalari Skullcharger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll, 120379 - Mod Scale 95-100%)
(@CGUID+146, 60384, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4256.24072265625, 1348.789794921875, 474.8348388671875, 1.156519770622253417, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Zandalari Pterror Wing (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+147, 60558, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4236.4677734375, 1416.1875, 454.417266845703125, 2.417953729629516601, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+148, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4259.767578125, 1351.3541259765625, 454.236419677734375, 5.425824642181396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+149, 60512, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4238.64501953125, 1414.078125, 454.417266845703125, 0, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Spirit Totem (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+150, 60558, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4237.986328125, 1431.361083984375, 454.417266845703125, 3.778153181076049804, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+151, 60559, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4231.53125, 1426.7691650390625, 454.417266845703125, 3.615119695663452148, 7200, 0, 0, 2378, 0, 0, 0, 0, 0, 46366), -- Zandalari Spirit (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+152, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4266.57666015625, 1348.44970703125, 454.334808349609375, 5.624925136566162109, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+153, 60512, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4239.49072265625, 1432.79345703125, 454.417266845703125, 0, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Spirit Totem (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+154, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4260.12841796875, 1372.032958984375, 454.236419677734375, 5.425824642181396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+155, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4247.75, 1422.9410400390625, 445.459869384765625, 1.010724544525146484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+156, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4262.12353515625, 1415.796875, 445.46697998046875, 1.010724544525146484, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+157, 60384, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4274.6279296875, 1322.0084228515625, 467.74224853515625, 2.855365991592407226, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Zandalari Pterror Wing (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+158, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4272.0849609375, 1325.84375, 454.571380615234375, 4.136549949645996093, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+159, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4262.6728515625, 1435.138916015625, 445.459869384765625, 1.010724544525146484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+160, 60375, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4263.53125, 1419.7291259765625, 445.5419921875, 4.200409889221191406, 7200, 0, 0, 28536, 0, 0, 0, 0, 0, 46366), -- Zandalari Skullcharger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll)
(@CGUID+161, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4266.66650390625, 1417.857666015625, 445.46563720703125, 0.724951863288879394, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+162, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4284.32666015625, 1334.8853759765625, 454.334808349609375, 2.198361635208129882, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+163, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4263.74462890625, 1427.175048828125, 445.5120849609375, 0.319092720746994018, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+164, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4290.86474609375, 1333.71875, 454.57196044921875, 5.425824642181396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+165, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4264.53125, 1434.4012451171875, 445.510009765625, 2.763818979263305664, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+166, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4289.62939453125, 1364.9879150390625, 454.236419677734375, 5.425824642181396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+167, 61352, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4226.736328125, 1486.15625, 432.797454833984375, 1.514943957328796386, 7200, 0, 0, 4756, 0, 0, 0, 0, 0, 46366), -- Story Time (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+168, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4290.88671875, 1361.7281494140625, 454.231048583984375, 4.711600303649902343, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+169, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4276.95166015625, 1431.5382080078125, 445.459869384765625, 1.010724544525146484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+170, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4302.9296875, 1348.796875, 454.236419677734375, 5.425824642181396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: )
(@CGUID+171, 60381, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4301.5986328125, 1347.3043212890625, 454.2310791015625, 0.842404305934906005, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Zandalari Infiltrator (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123167 - Mod Scale 95-100%)
(@CGUID+172, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4283.32470703125, 1413.7847900390625, 445.46435546875, 2.400677204132080078, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+173, 60375, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4289.6787109375, 1418.5260009765625, 445.533111572265625, 5.258900165557861328, 7200, 0, 0, 28536, 0, 0, 0, 0, 0, 46366), -- Zandalari Skullcharger (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117985 - Troll Door Smasher Recoil, 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll)
(@CGUID+174, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4303.75537109375, 1418.29345703125, 445.459869384765625, 1.010724544525146484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+175, 60644, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4296.033203125, 1437.1475830078125, 445.459869384765625, 1.010724544525146484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Troll Explosives (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+176, 60402, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4297.30712890625, 1420.1944580078125, 445.46697998046875, 2.50315093994140625, 7200, 0, 0, 15216, 23499, 0, 0, 0, 0, 46366), -- Zandalari Fire-Dancer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117826 - Fire Channeling)
(@CGUID+177, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4104.02880859375, 1537.46533203125, 378.833160400390625, 4.696813583374023437, 7200, 10, 0, 71340, 11037, 1, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn)) (possible waypoints or random movement)
(@CGUID+178, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4095.36279296875, 1533.342041015625, 378.91650390625, 4.843317985534667968, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+179, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4051.020751953125, 1533.0069580078125, 378.916534423828125, 4.392596721649169921, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+180, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4034.107666015625, 1533.263916015625, 378.9161376953125, 4.540678024291992187, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+181, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4093.919677734375, 1550.2242431640625, 378.8248291015625, 3.139675140380859375, 7200, 10, 0, 71340, 11037, 1, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn)) (possible waypoints or random movement)
(@CGUID+182, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4044.56591796875, 1539.513916015625, 378.91668701171875, 4.618653297424316406, 7200, 0, 0, 71340, 11037, 0, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+183, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4231.4365234375, 1341.7708740234375, 465.7745361328125, 6.274838924407958984, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+184, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4242.71337890625, 1307.2535400390625, 465.7745361328125, 0.782997608184814453, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+185, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4248.4306640625, 1313.657958984375, 485.55389404296875, 0.782997608184814453, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+186, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4244.4609375, 1374.79345703125, 465.7745361328125, 5.517777919769287109, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+187, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4236.9296875, 1341.7708740234375, 485.55389404296875, 6.274838924407958984, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+188, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4249.0849609375, 1369.953125, 485.55389404296875, 5.517777919769287109, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+189, 64529, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4277.09033203125, 1341.35595703125, 454.5430908203125, 1.570796370506286621, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Horizontal Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126038 - Mogu Runes (Horizontal))
(@CGUID+190, 64526, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4277.09033203125, 1341.35595703125, 454.5430908203125, 0, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Ancient Mogu Artifact (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126029 - Achievement Verification Aura)
(@CGUID+191, 65228, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4277.3349609375, 1341.8055419921875, 500.44580078125, 0, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 180) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127567 - Mogu Runes (Pitch Forward 180) DNT)
(@CGUID+192, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4276.9921875, 1387.5382080078125, 465.7745361328125, 4.707700252532958984, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+193, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4276.9921875, 1381.795166015625, 485.55389404296875, 4.707700252532958984, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+194, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4276.9921875, 1293.5191650390625, 465.7745361328125, 1.548142433166503906, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+195, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4276.55908203125, 1301.6285400390625, 485.55389404296875, 1.548142433166503906, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+196, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4310.11474609375, 1373.953125, 465.7745361328125, 3.918613433837890625, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+197, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4305.42724609375, 1369.1754150390625, 485.55389404296875, 3.918613433837890625, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+198, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4310.76904296875, 1306.6441650390625, 465.7745361328125, 2.310093641281127929, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+199, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4304.83056640625, 1312.9635009765625, 485.55389404296875, 2.310093641281127929, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+200, 64600, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4323.97412109375, 1340.703125, 465.7745361328125, 3.103055715560913085, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Vertical Visual) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 126183 - Mogu Runes (Vertical))
(@CGUID+201, 65227, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4316.869140625, 1340.9010009765625, 485.55389404296875, 3.103055715560913085, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Mogu Runes (Pitch Forward 120) DNT (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127566 - Mogu Runes (Pitch Forward 120) DNT)
(@CGUID+202, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4292.1552734375, 1534.095458984375, 438.883056640625, 4.720319271087646484, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 131378 - Story Time)
(@CGUID+203, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4231.50537109375, 1606.1195068359375, 410.48577880859375, 1.520822048187255859, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+204, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4233.05224609375, 1610.8785400390625, 410.54296875, 0, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+205, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4228.16015625, 1612.3597412109375, 410.600982666015625, 3.67842721939086914, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+206, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4216.10498046875, 1601.9935302734375, 410.42999267578125, 4.38775491714477539, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+207, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4095.421875, 1567.560791015625, 378.91650390625, 1.543448805809020996, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+208, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4176.310546875, 1554.002197265625, 383.231048583984375, 4.903876304626464843, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+209, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4176.14208984375, 1546.961181640625, 383.249603271484375, 2.972184181213378906, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+210, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4172.07470703125, 1552.5255126953125, 383.327301025390625, 4.338166713714599609, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+211, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4171.5, 1543.6900634765625, 383.2310791015625, 0.971139013767242431, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+212, 61345, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4265.91064453125, 1532.0660400390625, 427.319671630859375, 3.144654273986816406, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Mogu Archer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+213, 61429, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4159.3974609375, 1519.3853759765625, 422.825164794921875, 0.052063733339309692, 7200, 0, 0, 196800, 100, 0, 0, 0, 0, 46366), -- Meng the Demented (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+214, 61347, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4165.88720703125, 1513.7222900390625, 423.846771240234375, 0.013956028968095779, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Kingsguard (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+215, 61341, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4213.8427734375, 1549.076416015625, 424.018157958984375, 5.42813730239868164, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Mounted Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+216, 61250, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4188.11962890625, 1495.420166015625, 424.018096923828125, 1.478790521621704101, 7200, 0, 0, 19024, 1142, 0, 0, 0, 0, 46366), -- Sorcerer Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+217, 61250, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4207.3349609375, 1494.2535400390625, 424.018096923828125, 1.478790521621704101, 7200, 0, 0, 19024, 1142, 0, 0, 0, 0, 46366), -- Sorcerer Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+218, 61427, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4259.73681640625, 1532.2882080078125, 422.960845947265625, 3.163126945495605468, 7200, 0, 0, 196800, 0, 0, 0, 0, 0, 46366), -- Subetai the Swift (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+219, 61250, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4183.1943359375, 1495.0711669921875, 424.018096923828125, 1.584267139434814453, 7200, 0, 0, 19024, 1142, 0, 0, 0, 0, 46366), -- Sorcerer Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+220, 61341, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4241.10498046875, 1548.953125, 424.017822265625, 3.892516136169433593, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Mounted Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+221, 61345, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4272.90625, 1532.09033203125, 431.59112548828125, 3.144654273986816406, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Mogu Archer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+222, 61347, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4165.6865234375, 1525.2135009765625, 423.8680419921875, 0.013956028968095779, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Kingsguard (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+223, 61250, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4201.60400390625, 1494.65283203125, 424.018096923828125, 1.478790521621704101, 7200, 0, 0, 19024, 1142, 0, 0, 0, 0, 46366), -- Sorcerer Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+224, 61421, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4191.9443359375, 1491.24658203125, 423.018096923828125, 1.493658423423767089, 7200, 0, 0, 196800, 118900, 0, 0, 0, 0, 46366), -- Zian of the Endless Shadow (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+225, 61423, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4227.69091796875, 1553.02783203125, 423.0478515625, 4.763572216033935546, 7200, 0, 0, 196800, 0, 0, 0, 0, 0, 46366), -- Qiang the Merciless (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+226, 61345, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4268.869140625, 1535.4757080078125, 429.461273193359375, 3.144654273986816406, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Mogu Archer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+227, 61250, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.5146484375, 1495.251708984375, 424.018096923828125, 1.478790521621704101, 7200, 0, 0, 19024, 1142, 0, 0, 0, 0, 46366), -- Sorcerer Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+228, 61345, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4269.36376953125, 1527.6475830078125, 429.8890380859375, 3.144654273986816406, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Mogu Archer (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+229, 61347, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4167.29541015625, 1519.27783203125, 423.851165771484375, 0.013956028968095779, 7200, 0, 0, 19024, 0, 0, 0, 0, 0, 46366), -- Kingsguard (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 119310 - Inactive Visual)
(@CGUID+230, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4044.591064453125, 1560.875, 378.916656494140625, 1.493495941162109375, 7200, 0, 0, 71340, 11037, 0, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+231, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4140.7138671875, 1532.6634521484375, 383.265625, 4.065943717956542968, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+232, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4140.5703125, 1538.341796875, 383.255645751953125, 1.943420767784118652, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+233, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4136.630859375, 1533.423095703125, 383.30413818359375, 4.520579338073730468, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+234, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4051.0166015625, 1567.5833740234375, 378.916534423828125, 1.856082677841186523, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+235, 61139, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4034.0625, 1567.3333740234375, 378.916473388671875, 1.651529550552368164, 7200, 0, 0, 11110, 1067, 0, 0, 0, 0, 46366), -- Shan-xi Watershaper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling)
(@CGUID+236, 59677, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4101.947265625, 1517.0867919921875, 375.73065185546875, 0, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2, Flying) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118595 - Water Cosmetic)
(@CGUID+237, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4135.1640625, 1569.476318359375, 383.2828369140625, 3.333217144012451171, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+238, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4054.24267578125, 1876.6373291015625, 358.87164306640625, 5.497786998748779296, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+239, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4163.0078125, 1604.46875, 396.8363037109375, 1.332378625869750976, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+240, 65343, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4165.015625, 1607.218017578125, 396.836273193359375, 0.418479144573211669, 7200, 10, 0, 5, 0, 1, 0, 0, 0, 46366), -- Bone Spider (Area: Mogu'shan Vaults - Difficulty: 3) (possible waypoints or random movement)
(@CGUID+241, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4067.1298828125, 1907.75, 358.87164306640625, 0, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+242, 60709, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.3349609375, 1626.28125, 438.856292724609375, 4.723484516143798828, 7200, 0, 0, 246000, 0, 0, 0, 0, 0, 46366), -- Qiang the Merciless (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117920 - Massive Attacks)
(@CGUID+243, 60710, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4257.36279296875, 1591.3663330078125, 438.841064453125, 3.135262727737426757, 7200, 0, 0, 246000, 0, 0, 0, 0, 0, 46366), -- Subetai the Swift (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118205 - Inactive Visual)
(@CGUID+244, 60984, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4227.09130859375, 1594.1614990234375, 438.840850830078125, 4.688892364501953125, 7200, 0, 0, 4756, 0, 0, 0, 0, 0, 46366), -- Controller (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+245, 60701, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.9697265625, 1558.3367919921875, 438.839935302734375, 1.586655974388122558, 7200, 0, 0, 246000, 118900, 0, 0, 0, 0, 46366), -- Zian of the Endless Shadow (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118205 - Inactive Visual)
(@CGUID+246, 60708, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4198.7978515625, 1590.2882080078125, 438.841400146484375, 6.26344919204711914, 7200, 0, 0, 246000, 100, 0, 0, 0, 0, 46366), -- Meng the Demented (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117707 - Energy Drain)
(@CGUID+247, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3992.017333984375, 1876.6373291015625, 358.87164306640625, 3.926990747451782226, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+248, 65297, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4023.1298828125, 1907.75, 378.083343505859375, 4.712388992309570312, 7200, 0, 0, 2222, 0, 0, 0, 0, 0, 46366), -- Invisible Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+249, 64061, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4064.62255859375, 1692.131591796875, 363.430023193359375, 0.000048069221520563, 7200, 10, 0, 47560, 1142, 1, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn), 128807 - Mogu Patrol Cosmetic) (possible waypoints or random movement)
(@CGUID+250, 64947, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4058.51220703125, 1700.1314697265625, 363.4300537109375, 0.00003356718661962, 7200, 10, 0, 47560, 1142, 1, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn), 128807 - Mogu Patrol Cosmetic) (possible waypoints or random movement)
(@CGUID+251, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4054.24267578125, 1938.8626708984375, 358.87164306640625, 0.78539818525314331, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+252, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3979.1298828125, 1907.75, 358.87164306640625, 3.141592741012573242, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+253, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4196.93212890625, 1571.9930419921875, 438.839111328125, 0.648668289184570312, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+254, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4198.7353515625, 1623.157958984375, 438.849853515625, 5.496047496795654296, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+255, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4217.9912109375, 1556.0208740234375, 438.838623046875, 1.342384934425354003, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+256, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4250.064453125, 1629.0833740234375, 438.844329833984375, 4.120392322540283203, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+257, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4231.6943359375, 1555.0035400390625, 438.83880615234375, 1.688141584396362304, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+258, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4259.81103515625, 1597.357666015625, 438.8409423828125, 3.219679594039916992, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+259, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4258.92626953125, 1591.29345703125, 438.840789794921875, 3.032473325729370117, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+260, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4213.908203125, 1556.7274169921875, 438.83740234375, 1.239421844482421875, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+261, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4250.1728515625, 1563.560791015625, 438.8369140625, 2.209077119827270507, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+262, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4209.939453125, 1558.7847900390625, 438.8365478515625, 1.128208756446838378, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+263, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4245.73779296875, 1631.657958984375, 438.842620849609375, 4.242250442504882812, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+264, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.38525390625, 1595.6336669921875, 438.84124755859375, 6.256527900695800781, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+265, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4192.61181640625, 1600.6788330078125, 438.84442138671875, 6.113621711730957031, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+266, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.8974609375, 1555.0086669921875, 438.83892822265625, 1.568119287490844726, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+267, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4260.6083984375, 1618.46533203125, 438.842529296875, 3.755338430404663085, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+268, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4222.2021484375, 1555.517333984375, 438.838958740234375, 1.449133515357971191, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+269, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4263.33349609375, 1613.6441650390625, 438.841339111328125, 3.620293617248535156, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+270, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4206.5234375, 1561.5885009765625, 438.83660888671875, 1.018080711364746093, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+271, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.1884765625, 1576.21533203125, 438.83984375, 0.528454244136810302, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+272, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4193.9931640625, 1581.0347900390625, 438.840423583984375, 0.394825339317321777, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+273, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4264.609375, 1608.7413330078125, 438.8426513671875, 3.496831178665161132, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+274, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.251953125, 1590.8489990234375, 438.8408203125, 0.123506225645542144, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+275, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4241.27783203125, 1558.217041015625, 438.836944580078125, 1.94288945198059082, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+276, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4245.7509765625, 1561.109375, 438.83673095703125, 2.078688383102416992, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+277, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4255.0693359375, 1573.32470703125, 438.83953857421875, 2.488651037216186523, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+278, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4200.06005859375, 1568.314208984375, 438.838531494140625, 0.776641666889190673, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+279, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.37158203125, 1619.046875, 438.848663330078125, 5.629010677337646484, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+280, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4194.994140625, 1586.0555419921875, 438.8404541015625, 0.26639866828918457, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+281, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4241.140625, 1634.234375, 438.8455810546875, 4.368252277374267578, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+282, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4207.11474609375, 1629.8038330078125, 438.85125732421875, 5.228990077972412109, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+283, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4253.07666015625, 1568.123291015625, 438.8387451171875, 2.344912290573120117, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+284, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4231.42724609375, 1635.7274169921875, 438.849151611328125, 4.604761600494384765, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+285, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4203.1650390625, 1564.4913330078125, 438.837158203125, 0.9041634202003479, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+286, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4190.580078125, 1604.282958984375, 438.85003662109375, 6.02825021743774414, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+287, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4217.01318359375, 1633.8125, 438.8529052734375, 4.963041305541992187, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+288, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4190.84375, 1609.6353759765625, 438.8470458984375, 5.89363718032836914, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+289, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4236.423828125, 1635.51220703125, 438.847076416015625, 4.485071182250976562, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+290, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4265.55712890625, 1603.04345703125, 438.84228515625, 3.352471828460693359, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+291, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4257.361328125, 1584.796875, 438.840576171875, 2.823559999465942382, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+292, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4202.6806640625, 1626.5711669921875, 438.8514404296875, 5.365647315979003906, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+293, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4257.26025390625, 1579.0504150390625, 438.840362548828125, 2.66189122200012207, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+294, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4211.8837890625, 1632.07470703125, 438.85272216796875, 5.097667217254638671, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+295, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4192.8203125, 1614.5711669921875, 438.848876953125, 5.758609771728515625, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+296, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4253.703125, 1625.7725830078125, 438.84466552734375, 4.00110626220703125, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+297, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4221.74462890625, 1635.25, 438.85076904296875, 4.841653347015380859, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+298, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4257.0791015625, 1622.407958984375, 438.844879150390625, 3.884418487548828125, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+299, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.52099609375, 1635.6492919921875, 438.851318359375, 4.724215507507324218, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+300, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4236.48974609375, 1556.30908203125, 438.838226318359375, 1.812473416328430175, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+301, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4190.580078125, 1604.282958984375, 438.85003662109375, 6.02825021743774414, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+302, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4190.84375, 1609.6353759765625, 438.8470458984375, 5.89363718032836914, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+303, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4192.8203125, 1614.5711669921875, 438.848876953125, 5.758609771728515625, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+304, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4195.37158203125, 1619.046875, 438.848663330078125, 5.629010677337646484, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+305, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4198.7353515625, 1623.157958984375, 438.849853515625, 5.496047496795654296, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+306, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4202.6806640625, 1626.5711669921875, 438.8514404296875, 5.365647315979003906, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+307, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4245.73779296875, 1631.657958984375, 438.842620849609375, 4.242250442504882812, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+308, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4253.703125, 1625.7725830078125, 438.84466552734375, 4.00110626220703125, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+309, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4257.0791015625, 1622.407958984375, 438.844879150390625, 3.884418487548828125, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+310, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4236.423828125, 1635.51220703125, 438.847076416015625, 4.485071182250976562, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+311, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4241.140625, 1634.234375, 438.8455810546875, 4.368252277374267578, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+312, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4250.064453125, 1629.0833740234375, 438.844329833984375, 4.120392322540283203, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+313, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4217.01318359375, 1633.8125, 438.8529052734375, 4.963041305541992187, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+314, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4221.74462890625, 1635.25, 438.85076904296875, 4.841653347015380859, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+315, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.52099609375, 1635.6492919921875, 438.851318359375, 4.724215507507324218, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+316, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4231.42724609375, 1635.7274169921875, 438.849151611328125, 4.604761600494384765, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+317, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4207.11474609375, 1629.8038330078125, 438.85125732421875, 5.228990077972412109, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+318, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4211.8837890625, 1632.07470703125, 438.85272216796875, 5.097667217254638671, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+319, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4260.6083984375, 1618.46533203125, 438.842529296875, 3.755338430404663085, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+320, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4263.33349609375, 1613.6441650390625, 438.841339111328125, 3.620293617248535156, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+321, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4258.92626953125, 1591.29345703125, 438.840789794921875, 3.032473325729370117, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+322, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4264.609375, 1608.7413330078125, 438.8426513671875, 3.496831178665161132, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+323, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4265.55712890625, 1603.04345703125, 438.84228515625, 3.352471828460693359, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+324, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4259.81103515625, 1597.357666015625, 438.8409423828125, 3.219679594039916992, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+325, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4226.8974609375, 1555.0086669921875, 438.83892822265625, 1.568119287490844726, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+326, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4222.2021484375, 1555.517333984375, 438.838958740234375, 1.449133515357971191, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+327, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4241.27783203125, 1558.217041015625, 438.836944580078125, 1.94288945198059082, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+328, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4236.48974609375, 1556.30908203125, 438.838226318359375, 1.812473416328430175, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+329, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4217.9912109375, 1556.0208740234375, 438.838623046875, 1.342384934425354003, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+330, 60847, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4231.6943359375, 1555.0035400390625, 438.83880615234375, 1.688141584396362304, 7200, 0, 0, 492000, 0, 0, 0, 0, 0, 46366), -- Flanking Mogu (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117904 - Ghostly Visual)
(@CGUID+331, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4224.93212890625, 1554.84033203125, 424.018157958984375, 1.478853821754455566, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+332, 59677, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4101.830078125, 1596.02783203125, 376.200836181640625, 0, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2, Flying) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118595 - Water Cosmetic)
(@CGUID+333, 60776, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3992.017333984375, 1938.8626708984375, 358.87164306640625, 2.356194496154785156, 7200, 0, 0, 7777, 0, 0, 0, 0, 0, 46366), -- Empyreal Focus (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127303 - Focus State Cosmetic - Initial Inactive State)
(@CGUID+334, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3955.47314453125, 1537.9097900390625, 369.644866943359375, 2.991392374038696289, 7200, 0, 0, 71340, 11037, 0, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+335, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3951.66064453125, 1549.9149169921875, 369.644866943359375, 3.102116107940673828, 7200, 0, 0, 71340, 11037, 0, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+336, 61131, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3955.403564453125, 1563.0885009765625, 369.6448974609375, 3.141077041625976562, 7200, 0, 0, 71340, 11037, 0, 0, 0, 0, 46366), -- Mogu'shan Secret-Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+337, 59677, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4044.859375, 1595.623291015625, 376.139892578125, 0, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2, Flying) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118595 - Water Cosmetic)
(@CGUID+338, 59677, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4044.1572265625, 1518.4166259765625, 376.106689453125, 0, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2, Flying) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118595 - Water Cosmetic)
(@CGUID+339, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3894.257080078125, 1615.7691650390625, 429.319488525390625, 4.668477058410644531, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+340, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3894.9306640625, 1615.9254150390625, 459.92388916015625, 4.624879837036132812, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+341, 64063, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4023.233642578125, 1828.2847900390625, 358.857818603515625, 4.617073535919189453, 7200, 0, 0, 32103, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Arcanist (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+342, 64947, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4030.537353515625, 1829.0052490234375, 358.866607666015625, 4.617073535919189453, 7200, 0, 0, 47560, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+343, 64061, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4015.549560546875, 1828.8004150390625, 358.86676025390625, 4.617073535919189453, 7200, 0, 0, 47560, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+344, 64068, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4023.673583984375, 1844.9896240234375, 358.8717041015625, 4.714232921600341796, 7200, 0, 0, 57072, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Engine Keeper (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+345, 64947, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4029.979248046875, 1847.6822509765625, 358.875244140625, 4.714232921600341796, 7200, 0, 0, 47560, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+346, 64061, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4017.1337890625, 1847.62158203125, 358.874786376953125, 4.714232921600341796, 7200, 0, 0, 47560, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Warden (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+347, 64063, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4036.970458984375, 1853.2291259765625, 358.871917724609375, 4.714232921600341796, 7200, 0, 0, 32103, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Arcanist (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+348, 64063, 1008, 6125, 6125, '0', '0', 0, 0, 1, 4009.41064453125, 1852.467041015625, 358.87298583984375, 4.714232921600341796, 7200, 0, 0, 32103, 1142, 0, 0, 0, 0, 46366), -- Mogu'shan Arcanist (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 123585 - Spirit Particles (Spawn))
(@CGUID+349, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3873.9296875, 1615.6041259765625, 429.31951904296875, 4.685750961303710937, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+350, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3988.481689453125, 1873.101806640625, 358.871673583984375, 3.919168472290039062, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+351, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4057.7783203125, 1873.101806640625, 358.871673583984375, 5.485804557800292968, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+352, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3974.1298828125, 1907.75, 358.8717041015625, 3.14504241943359375, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+353, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4072.1298828125, 1907.75, 358.8717041015625, 0.014556838199496269, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+354, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4048.2109375, 1696.9010009765625, 363.382415771484375, 3.144818305969238281, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+355, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3988.481689453125, 1942.398193359375, 358.871673583984375, 2.281279325485229492, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+356, 60960, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4057.7783203125, 1942.398193359375, 358.871673583984375, 0.752777636051177978, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Core Target Stalker (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+357, 61330, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4023.1298828125, 1907.75, 358.87164306640625, 4.712388992309570312, 7200, 0, 0, 22220, 0, 0, 0, 0, 0, 46366), -- Energy Vortex Stalker (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 118983 - Charge Vortex)
(@CGUID+358, 60410, 1008, 6125, 6125, '0', '0', 0, 0, 0, 4023.1298828125, 1907.75, 368.083343505859375, 4.712388992309570312, 7200, 0, 0, 1660500, 0, 0, 0, 0, 0, 46366), -- Elegon (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127808 - Elegon Spawn Cosmetic)
(@CGUID+359, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.217041015625, 1616.5538330078125, 490.000244140625, 4.610419273376464843, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+360, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3873.6025390625, 1616.078125, 459.9239501953125, 4.705873489379882812, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+361, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.471435546875, 1610.7691650390625, 429.319580078125, 5.071576595306396484, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+362, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3873.53125, 1615.49658203125, 490.000244140625, 4.7100067138671875, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+363, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3852.41845703125, 1611.4444580078125, 459.9239501953125, 4.99080657958984375, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+364, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3833.05029296875, 1600.6614990234375, 429.319580078125, 5.276216030120849609, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+365, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3818.478271484375, 1581.670166015625, 429.322540283203125, 5.658397197723388671, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+366, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.041748046875, 1616.1910400390625, 520.6046142578125, 4.619256973266601562, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+367, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3810.6962890625, 1550.4288330078125, 459.344970703125, 6.274814605712890625, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+368, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3811.8828125, 1536.7413330078125, 459.345977783203125, 0.175917237997055053, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+369, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3810.64404296875, 1550.4757080078125, 428.73736572265625, 6.272654056549072265, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+370, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.116455078125, 1564.0208740234375, 428.7384033203125, 5.779236316680908203, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+371, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3873.65625, 1616.4114990234375, 520.6046142578125, 4.702874183654785156, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+372, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3832.72216796875, 1600.282958984375, 459.92401123046875, 5.310358524322509765, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+373, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.55810546875, 1563.5972900390625, 459.345977783203125, 5.787673473358154296, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+374, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3811.9541015625, 1536.80908203125, 428.7384033203125, 0.172334223985671997, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+375, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3817.404541015625, 1582.4739990234375, 459.927001953125, 5.656612873077392578, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+376, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.520751953125, 1611.0382080078125, 490.000274658203125, 5.061381340026855468, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+377, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3810.693603515625, 1550.220458984375, 489.420196533203125, 0.001345961587503552, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+378, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3832.1259765625, 1600.6875, 490.000274658203125, 5.328369617462158203, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+379, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3816.552978515625, 1583.32470703125, 490.003265380859375, 5.643594264984130859, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+380, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.412353515625, 1563.638916015625, 489.41912841796875, 5.789409160614013671, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+381, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3851.190185546875, 1612.5347900390625, 520.60467529296875, 5.048283100128173828, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+382, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3811.7265625, 1536.6754150390625, 489.41912841796875, 0.178362548351287841, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+383, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3832.15966796875, 1601.0364990234375, 520.60467529296875, 5.313074588775634765, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+384, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3988.66064453125, 1454.0242919921875, 364.971160888671875, 3.204643487930297851, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366), -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+385, 60648, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3869.55029296875, 1550.4271240234375, 362.282440185546875, 6.249711513519287109, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Ancient Mogu Machine (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+386, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3869.643310546875, 1550.373291015625, 362.282470703125, 3.059442520141601562, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+387, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3811.3203125, 1536.2916259765625, 520.02667236328125, 0.197641178965568542, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+388, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3817.885498046875, 1517.3992919921875, 520.607666015625, 0.503707110881805419, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+389, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.52783203125, 1563.6597900390625, 520.02667236328125, 5.785509586334228515, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+390, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3810.6796875, 1550.3004150390625, 520.02777099609375, 6.280808448791503906, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic)
(@CGUID+391, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3816.989501953125, 1583.015625, 520.607666015625, 5.642691612243652343, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic)
(@CGUID+392, 60648, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3869.55029296875, 1550.4271240234375, 362.282440185546875, 6.249711513519287109, 7200, 0, 0, 2460, 0, 0, 0, 0, 0, 46366), -- Ancient Mogu Machine (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+393, 55091, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3869.643310546875, 1550.373291015625, 362.282470703125, 3.059442520141601562, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI) (Area: Mogu'shan Vaults - Difficulty: 3)
(@CGUID+394, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3983.103271484375, 1516.4410400390625, 369.644775390625, 3.144255876541137695, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+395, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3982.990478515625, 1567.8367919921875, 369.644775390625, 3.047890901565551757, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+396, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3983.2275390625, 1533.29345703125, 369.644775390625, 3.152572870254516601, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+397, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3982.842041015625, 1500.0052490234375, 369.644775390625, 3.126979351043701171, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+398, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3982.842041015625, 1600.7257080078125, 369.644775390625, 3.140606164932250976, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+399, 54020, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3982.966064453125, 1584.2396240234375, 369.644775390625, 3.154506921768188476, 7200, 0, 0, 18771, 0, 0, 0, 0, 0, 46366), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 127700 - Frost Visual)
(@CGUID+400, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.83251953125, 1563.6163330078125, 367.726470947265625, 5.77969217300415039, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored)
(@CGUID+401, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3852.390625, 1489.4566650390625, 398.912017822265625, 1.189559459686279296, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate)
(@CGUID+402, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3813.348876953125, 1537.357666015625, 367.726470947265625, 0.150286436080932617, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored)
(@CGUID+403, 60398, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3895.319580078125, 1614.3992919921875, 398.911956787109375, 4.637409210205078125, 7200, 0, 0, 10463, 0, 0, 0, 0, 0, 46366), -- Emperor's Courage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116537 - Half Plate)
(@CGUID+404, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3852.298583984375, 1490.361083984375, 368.3076171875, 1.05588531494140625, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate)
(@CGUID+405, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3813.00341796875, 1536.84375, 398.334014892578125, 0.18267899751663208, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored)
(@CGUID+406, 60400, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3818.5087890625, 1517.5382080078125, 368.327728271484375, 0.515781342983245849, 7200, 0, 0, 885600, 100, 0, 0, 0, 0, 46366), -- Jan-xi (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 117193 - Magnetic Armor)
(@CGUID+407, 60399, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3818.123291015625, 1582.8333740234375, 368.327728271484375, 5.601917743682861328, 7200, 0, 0, 885600, 100, 0, 0, 0, 0, 46366), -- Qin-xi (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116815 - Magnetic Armor)
(@CGUID+408, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3812.12158203125, 1550.07470703125, 398.3330078125, 0.008728244341909885, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored)
(@CGUID+409, 60397, 1008, 6125, 6125, '0', '0', 0, 0, 1, 3833.604248046875, 1600.8819580078125, 398.91204833984375, 5.334136009216308593, 7200, 0, 0, 23780, 0, 0, 0, 0, 0, 46366), -- Emperor's Strength (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116540 - Full Plate)
(@CGUID+410, 60396, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3811.307373046875, 1550.232666015625, 367.72540283203125, 0.000802144117187708, 7200, 0, 0, 9512, 0, 0, 0, 0, 0, 46366), -- Emperor's Rage (Area: Mogu'shan Vaults - Difficulty: 3) (Auras: 116535 - Unarmored)
(@CGUID+411, 61348, 1008, 6125, 6125, '0', '0', 0, 0, 0, 3988.66064453125, 1454.0242919921875, 364.971160888671875, 3.151058435440063476, 7200, 0, 0, 142680, 0, 0, 0, 0, 0, 46366); -- Lorewalker Cho (Area: Mogu'shan Vaults - Difficulty: 3)

DELETE FROM creature_addon WHERE guid BETWEEN @CGUID+0 AND @CGUID+411;
INSERT INTO creature_addon (guid, path_id, mount, bytes1, bytes2, emote, aiAnimKit, movementAnimKit, meleeAnimKit, visibilityDistanceType, auras) VALUES
(@CGUID+0, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+1, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816'), -- Enormous Stone Quilen - 126816 - Dampening Field
(@CGUID+2, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+3, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+4, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+5, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+6, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Leng Windstaff
(@CGUID+7, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+8, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+9, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+10, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+11, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+12, 0, 0, 0, 1, 0, 0, 0, 0, 4, ''), -- Invisible Stalker
(@CGUID+13, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+14, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+15, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+16, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+17, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+18, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+19, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+20, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+21, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+22, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+23, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+24, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+25, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+26, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+27, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+28, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+29, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+30, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+31, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+32, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+33, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+34, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+35, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+36, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+37, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816'), -- Enormous Stone Quilen - 126816 - Dampening Field
(@CGUID+38, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+39, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+40, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+41, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+42, 0, 0, 0, 1, 0, 0, 0, 0, 4, '126816'), -- Stone Guard Controller - 126816 - Dampening Field
(@CGUID+43, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+44, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+45, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+46, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+47, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+48, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+49, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+50, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+51, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+52, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+53, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+54, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+55, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+56, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- Stone Quilen - 126816 - Dampening Field, 117838 - Stone Form
(@CGUID+57, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+58, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+59, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+60, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- Jade Guardian - 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit
(@CGUID+61, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- Amethyst Guardian - 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit
(@CGUID+62, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- Jasper Guardian - 72242 - Zero Energy + Zero Regen, 115745 - Solid Stone, 128886 - Quilen Freeze Sit Anim Kit
(@CGUID+63, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+64, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+65, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+66, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+67, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+68, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+69, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+70, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+71, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- World Trigger
(@CGUID+73, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120661 120613'), -- Cursed Mogu Sculpture - 120661 - Bronze, 120613 - Pose
(@CGUID+74, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120661 120613'), -- Cursed Mogu Sculpture - 120661 - Bronze, 120613 - Pose
(@CGUID+75, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+76, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120681'), -- Ghost Essence - 120681 - Ghost Essence
(@CGUID+77, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120650'), -- Cursed Mogu Sculpture - 120663 - Stone, 120650 - Pose
(@CGUID+78, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+79, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120651'), -- Cursed Mogu Sculpture - 120663 - Stone, 120651 - Pose
(@CGUID+80, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+81, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+82, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120650'), -- Cursed Mogu Sculpture - 120663 - Stone, 120650 - Pose
(@CGUID+83, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120650'), -- Cursed Mogu Sculpture - 120663 - Stone, 120650 - Pose
(@CGUID+84, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+85, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120651'), -- Cursed Mogu Sculpture - 120663 - Stone, 120651 - Pose
(@CGUID+86, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+87, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- World Trigger
(@CGUID+88, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+89, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+90, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+91, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- World Trigger
(@CGUID+92, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+93, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+94, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+95, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+96, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+97, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+98, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
(@CGUID+99, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+100, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+101, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- World Trigger
(@CGUID+102, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+103, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+104, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+105, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+106, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+107, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+108, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+109, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+110, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+111, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+112, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+113, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+114, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+115, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+116, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+117, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+118, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+119, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+120, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+121, 0, 0, 0, 1, 0, 0, 0, 0, 0, '118486'), -- Phase Controller - 118486 - Phase Transition (Staff)
(@CGUID+122, 0, 0, 0, 1, 0, 0, 0, 0, 0, '118485'), -- Phase Controller - 118485 - Phase Transition (Spear)
(@CGUID+123, 0, 0, 0, 1, 0, 0, 0, 0, 0, '105032'), -- Phase Controller - 105032 - Phase Transition (Fist)
(@CGUID+124, 0, 0, 0, 1, 0, 0, 0, 0, 3, '124252'), -- Feng the Accursed - 124252 - Unarmed
(@CGUID+125, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Spirit Totem
(@CGUID+126, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+127, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Zandalari War Wyvern
(@CGUID+128, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+129, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Spirit Totem
(@CGUID+130, 0, 0, 0, 1, 0, 0, 0, 0, 3, '115964'), -- Gara'jal the Spiritbinder - 115964 - Mighty Mojo
(@CGUID+131, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+132, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Pterror Wing - 123167 - Mod Scale 95-100%
(@CGUID+133, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+134, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116545'), -- Troll Explosives - 116545 - Troll Explosives
(@CGUID+135, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+136, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+137, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116545'), -- Troll Explosives - 116545 - Troll Explosives
(@CGUID+138, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+139, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116545'), -- Troll Explosives - 116545 - Troll Explosives
(@CGUID+140, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+141, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+142, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+143, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+144, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167 116966 120379'), -- Zandalari Skullcharger - 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll, 120379 - Mod Scale 95-100%
(@CGUID+145, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167 116966 120379'), -- Zandalari Skullcharger - 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll, 120379 - Mod Scale 95-100%
(@CGUID+146, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Pterror Wing - 123167 - Mod Scale 95-100%
(@CGUID+147, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+148, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+149, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Spirit Totem
(@CGUID+150, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+151, 0, 0, 0, 1, 10, 0, 0, 0, 3, ''), -- Zandalari Spirit
(@CGUID+152, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+153, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Spirit Totem
(@CGUID+154, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+155, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+156, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+157, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Pterror Wing - 123167 - Mod Scale 95-100%
(@CGUID+158, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+159, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+160, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167 116966'), -- Zandalari Skullcharger - 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll
(@CGUID+161, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+162, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+163, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+164, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+165, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+166, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+167, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Story Time
(@CGUID+168, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+169, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+170, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+171, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- Zandalari Infiltrator - 123167 - Mod Scale 95-100%
(@CGUID+172, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+173, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117985 123167 116966'), -- Zandalari Skullcharger - 117985 - Troll Door Smasher Recoil, 123167 - Mod Scale 95-100%, 116966 - Zandalarian Big Troll
(@CGUID+174, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+175, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Troll Explosives
(@CGUID+176, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- Zandalari Fire-Dancer - 117826 - Fire Channeling
(@CGUID+177, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+178, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+179, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+180, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+181, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+182, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+183, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+184, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+185, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+186, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+187, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+188, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+189, 0, 0, 0, 1, 0, 0, 0, 0, 0, '126038'), -- Mogu Runes (Horizontal Visual) - 126038 - Mogu Runes (Horizontal)
(@CGUID+190, 0, 0, 0, 1, 0, 0, 0, 0, 0, '126029'), -- Ancient Mogu Artifact - 126029 - Achievement Verification Aura
(@CGUID+191, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127567'), -- Mogu Runes (Pitch Forward 180) DNT - 127567 - Mogu Runes (Pitch Forward 180) DNT
(@CGUID+192, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+193, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+194, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+195, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+196, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+197, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+198, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+199, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+200, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- Mogu Runes (Vertical Visual) - 126183 - Mogu Runes (Vertical)
(@CGUID+201, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- Mogu Runes (Pitch Forward 120) DNT - 127566 - Mogu Runes (Pitch Forward 120) DNT
(@CGUID+202, 0, 0, 0, 1, 0, 0, 0, 0, 3, '131378'), -- Lorewalker Cho - 131378 - Story Time
(@CGUID+203, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+204, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+205, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+206, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+207, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+208, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+209, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+210, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+211, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+212, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mogu Archer - 119310 - Inactive Visual
(@CGUID+213, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- Meng the Demented - 119310 - Inactive Visual
(@CGUID+214, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Kingsguard - 119310 - Inactive Visual
(@CGUID+215, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mounted Mogu - 119310 - Inactive Visual
(@CGUID+216, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Sorcerer Mogu - 119310 - Inactive Visual
(@CGUID+217, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Sorcerer Mogu - 119310 - Inactive Visual
(@CGUID+218, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- Subetai the Swift - 119310 - Inactive Visual
(@CGUID+219, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Sorcerer Mogu - 119310 - Inactive Visual
(@CGUID+220, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mounted Mogu - 119310 - Inactive Visual
(@CGUID+221, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mogu Archer - 119310 - Inactive Visual
(@CGUID+222, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Kingsguard - 119310 - Inactive Visual
(@CGUID+223, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Sorcerer Mogu - 119310 - Inactive Visual
(@CGUID+224, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- Zian of the Endless Shadow - 119310 - Inactive Visual
(@CGUID+225, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- Qiang the Merciless - 119310 - Inactive Visual
(@CGUID+226, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mogu Archer - 119310 - Inactive Visual
(@CGUID+227, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Sorcerer Mogu - 119310 - Inactive Visual
(@CGUID+228, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Mogu Archer - 119310 - Inactive Visual
(@CGUID+229, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- Kingsguard - 119310 - Inactive Visual
(@CGUID+230, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+231, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+232, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+233, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+234, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+235, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- Shan-xi Watershaper - 37119 - Spirit Particles (Spawn), 45846 - Frost Channelling
(@CGUID+236, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '118595'), -- General Purpose Bunny JMF (Look 2, Flying) - 118595 - Water Cosmetic
(@CGUID+237, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+238, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+239, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+240, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Bone Spider
(@CGUID+241, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+242, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117920'), -- Qiang the Merciless - 117920 - Massive Attacks
(@CGUID+243, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118205'), -- Subetai the Swift - 118205 - Inactive Visual
(@CGUID+244, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Controller
(@CGUID+245, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118205'), -- Zian of the Endless Shadow - 118205 - Inactive Visual
(@CGUID+246, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117707'), -- Meng the Demented - 117707 - Energy Drain
(@CGUID+247, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+248, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- Invisible Stalker
(@CGUID+249, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585 128807'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn), 128807 - Mogu Patrol Cosmetic
(@CGUID+250, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585 128807'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn), 128807 - Mogu Patrol Cosmetic
(@CGUID+251, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+252, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+253, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+254, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+255, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+256, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+257, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+258, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+259, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+260, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+261, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+262, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+263, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+264, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+265, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+266, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+267, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+268, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+269, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+270, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+271, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+272, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+273, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+274, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+275, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+276, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+277, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+278, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+279, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+280, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+281, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+282, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+283, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+284, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+285, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+286, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+287, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+288, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+289, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+290, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+291, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+292, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+293, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+294, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+295, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+296, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+297, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+298, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+299, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+300, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+301, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+302, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+303, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+304, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+305, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+306, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+307, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+308, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+309, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+310, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+311, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+312, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+313, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+314, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+315, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+316, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+317, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+318, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+319, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+320, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+321, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+322, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+323, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+324, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+325, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+326, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+327, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+328, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+329, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+330, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- Flanking Mogu - 117904 - Ghostly Visual
(@CGUID+331, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+332, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '118595'), -- General Purpose Bunny JMF (Look 2, Flying) - 118595 - Water Cosmetic
(@CGUID+333, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- Empyreal Focus - 127303 - Focus State Cosmetic - Initial Inactive State
(@CGUID+334, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+335, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+336, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Secret-Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+337, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '118595'), -- General Purpose Bunny JMF (Look 2, Flying) - 118595 - Water Cosmetic
(@CGUID+338, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '118595'), -- General Purpose Bunny JMF (Look 2, Flying) - 118595 - Water Cosmetic
(@CGUID+339, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+340, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+341, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Arcanist - 123585 - Spirit Particles (Spawn)
(@CGUID+342, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn)
(@CGUID+343, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn)
(@CGUID+344, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Engine Keeper - 123585 - Spirit Particles (Spawn)
(@CGUID+345, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn)
(@CGUID+346, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Warden - 123585 - Spirit Particles (Spawn)
(@CGUID+347, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Arcanist - 123585 - Spirit Particles (Spawn)
(@CGUID+348, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- Mogu'shan Arcanist - 123585 - Spirit Particles (Spawn)
(@CGUID+349, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+350, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+351, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+352, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+353, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+354, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+355, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+356, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Core Target Stalker
(@CGUID+357, 0, 0, 0, 1, 0, 0, 0, 0, 4, '118983'), -- Energy Vortex Stalker - 118983 - Charge Vortex
(@CGUID+358, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127808'), -- Elegon - 127808 - Elegon Spawn Cosmetic
(@CGUID+359, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+360, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+361, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+362, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+363, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+364, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+365, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+366, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+367, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+368, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+369, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+370, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+371, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+372, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+373, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+374, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+375, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+376, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+377, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+378, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+379, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+380, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+381, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+382, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+383, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- Emperor's Strength - 116540 - Full Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+384, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- Lorewalker Cho
(@CGUID+385, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Ancient Mogu Machine
(@CGUID+386, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+387, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+388, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+389, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+390, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- Emperor's Rage - 116535 - Unarmored, 118039 - Terracotta Stone Cosmetic
(@CGUID+391, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- Emperor's Courage - 116537 - Half Plate, 118039 - Terracotta Stone Cosmetic
(@CGUID+392, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- Ancient Mogu Machine
(@CGUID+393, 0, 0, 50331648, 1, 0, 0, 0, 0, 5, ''), -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
(@CGUID+394, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+395, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+396, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+397, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+398, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+399, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127700'), -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI) - 127700 - Frost Visual
(@CGUID+400, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535'), -- Emperor's Rage - 116535 - Unarmored
(@CGUID+401, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540'), -- Emperor's Strength - 116540 - Full Plate
(@CGUID+402, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535'), -- Emperor's Rage - 116535 - Unarmored
(@CGUID+403, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537'), -- Emperor's Courage - 116537 - Half Plate
(@CGUID+404, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540'), -- Emperor's Strength - 116540 - Full Plate
(@CGUID+405, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535'), -- Emperor's Rage - 116535 - Unarmored
(@CGUID+406, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117193'), -- Jan-xi - 117193 - Magnetic Armor
(@CGUID+407, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116815'), -- Qin-xi - 116815 - Magnetic Armor
(@CGUID+408, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535'), -- Emperor's Rage - 116535 - Unarmored
(@CGUID+409, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540'), -- Emperor's Strength - 116540 - Full Plate
(@CGUID+410, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535'), -- Emperor's Rage - 116535 - Unarmored
(@CGUID+411, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''); -- Lorewalker Cho

DELETE FROM gameobject WHERE guid BETWEEN @OGUID+0 AND @OGUID+52;
INSERT INTO gameobject (guid, id, map, zoneId, areaId, spawnDifficulties, PhaseId, PhaseGroup, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state, VerifiedBuild) VALUES
(@OGUID+0, 214441, 1008, 6125, 6125, '0', '0', 0, 3848.869873046875, 1044.6285400390625, 492.389068603515625, 0.019733749330043792, 0, 0, 0.009866714477539062, 0.999951303005218505, 7200, 255, 1, 46366), -- Instance Portal (Raid 4 Difficulties) (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+1, 211693, 1008, 6125, 6125, '0', '0', 0, 3745.49267578125, 539.72271728515625, 639.01324462890625, 3.019413232803344726, 0, 0, 0.998134613037109375, 0.061051756143569946, 7200, 255, 1, 46366), -- Inner Doors (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+2, 210868, 1008, 6125, 6125, '0', '0', 0, 3699.746337890625, 545.3397216796875, 639.62884521484375, 3.019413232803344726, 0, 0, 0.998134613037109375, 0.061051756143569946, 7200, 255, 1, 46366), -- Outer Doors (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+3, 213285, 1008, 6125, 6125, '0', '0', 0, 4277.06298828125, 1443.3426513671875, 445.3636474609375, 4.712389945983886718, 0, 0, -0.70710659027099609, 0.707106947898864746, 7200, 255, 0, 46366), -- Ancient Mogu Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+4, 214452, 1008, 6125, 6125, '0', '0', 0, 4005.69091796875, 1341.5, 466.3043212890625, 0.010906274430453777, 0, 0, 0.005453109741210937, 0.999985098838806152, 7200, 255, 0, 46366), -- Feng's Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+5, 214696, 1008, 6125, 6125, '0', '0', 0, 4177.4228515625, 1341.57470703125, 461.906585693359375, 0, 0, 0, 0, 1, 7200, 255, 0, 46366), -- Ancient Mogu Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+6, 213248, 1008, 6125, 6125, '0', '0', 0, 4014.90673828125, 1389.004638671875, 401.449005126953125, 5.323255538940429687, 0, 0, -0.46174812316894531, 0.887011110782623291, 7200, 255, 0, 46366), -- Doodad_Mogu_Statue_MoguShanVaults003 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+7, 214452, 1008, 6125, 6125, '0', '0', 0, 4077.350830078125, 1341.201416015625, 466.305084228515625, 0.010906274430453777, 0, 0, 0.005453109741210937, 0.999985098838806152, 7200, 255, 0, 46366), -- Feng's Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+8, 213245, 1008, 6125, 6125, '0', '0', 0, 4014.959716796875, 1295.42529296875, 401.449005126953125, 0.872663915157318115, 0, 0, 0.422617912292480468, 0.906307935714721679, 7200, 255, 2, 46366), -- Doodad_Mogu_Statue_MoguShanVaults002 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+9, 213258, 1008, 6125, 6125, '0', '0', 0, 3998.8818359375, 1550.3590087890625, 374.166412353515625, 0, 0, 0, 0, 1, 7200, 255, 0, 46366), -- Ancient Mogu Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+10, 213244, 1008, 6125, 6125, '0', '0', 0, 4072.940673828125, 1630.121337890625, 376.820159912109375, 4.712389945983886718, 0, 0, -0.70710659027099609, 0.707106947898864746, 7200, 255, 0, 46366), -- Ancient Mogu Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+11, 214526, 1008, 6125, 6125, '0', '0', 0, 3899.392333984375, 1290.803466796875, 470.850860595703125, 4.712389945983886718, 0, 0, -0.70710659027099609, 0.707106947898864746, 7200, 255, 0, 46366), -- Ancient Mogu Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+12, 213247, 1008, 6125, 6125, '0', '0', 0, 4069.479248046875, 1389.8052978515625, 401.449005126953125, 4.276057243347167968, 0, 0, -0.84339141845703125, 0.537299633026123046, 7200, 255, 2, 46366), -- Doodad_Mogu_Statue_MoguShanVaults004 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+13, 213246, 1008, 6125, 6125, '0', '0', 0, 4069.110107421875, 1293.3349609375, 401.449005126953125, 2.094393253326416015, 0, 0, 0.866024971008300781, 0.50000077486038208, 7200, 255, 2, 46366), -- Doodad_Mogu_Statue_MoguShanVaults001 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+14, 213953, 1008, 6125, 6125, '0', '0', 0, 4023.0634765625, 1880.7584228515625, 95.62939453125, 0, -0.6839151382446289, 0, 0, 0.729561567306518554, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+15, 213952, 1008, 6125, 6125, '0', '0', 0, 4023.256103515625, 1936.8076171875, 137.716705322265625, 3.143104076385498046, 0, 0.683915138244628906, -0.72956085205078125, 0.001035800902172923, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+16, 213955, 1008, 6125, 6125, '0', '0', 0, 4023.255859375, 1934.4378662109375, 95.62939453125, 3.143104076385498046, 0, 0.683915138244628906, -0.72956085205078125, 0.001035800902172923, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+17, 213954, 1008, 6125, 6125, '0', '0', 0, 3996.320068359375, 1907.6944580078125, 95.629364013671875, 4.712390422821044921, -0.48360109329223632, 0.483600616455078125, -0.51587772369384765, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+18, 213937, 1008, 6125, 6125, '0', '0', 0, 4023.255859375, 1943.657470703125, 260.9927978515625, 3.143104076385498046, 0, 0.683915138244628906, -0.72956085205078125, 0.001035800902172923, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+19, 213939, 1008, 6125, 6125, '0', '0', 0, 4023.063720703125, 1871.5390625, 260.992828369140625, 0, -0.6839151382446289, 0, 0, 0.729561567306518554, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+20, 213938, 1008, 6125, 6125, '0', '0', 0, 3987.1005859375, 1907.6943359375, 260.9927978515625, 4.712390422821044921, -0.48360109329223632, 0.483600616455078125, -0.51587772369384765, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+21, 213941, 1008, 6125, 6125, '0', '0', 0, 4057.376953125, 1907.501953125, 224.676483154296875, 1.570795059204101562, -0.48360109329223632, -0.48360061645507812, 0.515877723693847656, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+22, 213940, 1008, 6125, 6125, '0', '0', 0, 4059.218994140625, 1907.501953125, 260.9927978515625, 1.570795059204101562, -0.48360109329223632, -0.48360061645507812, 0.515877723693847656, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+23, 213943, 1008, 6125, 6125, '0', '0', 0, 3988.942626953125, 1907.6943359375, 224.676483154296875, 4.712390422821044921, -0.48360109329223632, 0.483600616455078125, -0.51587772369384765, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+24, 213942, 1008, 6125, 6125, '0', '0', 0, 4023.0634765625, 1873.381103515625, 224.676513671875, 0, -0.6839151382446289, 0, 0, 0.729561567306518554, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+25, 213945, 1008, 6125, 6125, '0', '0', 0, 4054.749755859375, 1907.501953125, 181.318328857421875, 1.570795059204101562, -0.48360109329223632, -0.48360061645507812, 0.515877723693847656, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+26, 213527, 1008, 6125, 6125, '0', '0', 0, 4023.15966796875, 1907.605712890625, 326.19659423828125, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 1, 46366), -- Titan Energy Ring (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+27, 213944, 1008, 6125, 6125, '0', '0', 0, 4023.255859375, 1941.8153076171875, 224.676483154296875, 3.143104076385498046, 0, 0.683915138244628906, -0.72956085205078125, 0.001035800902172923, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+28, 213947, 1008, 6125, 6125, '0', '0', 0, 3991.56982421875, 1907.6944580078125, 181.31829833984375, 4.712390422821044921, -0.48360109329223632, 0.483600616455078125, -0.51587772369384765, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+29, 213529, 1008, 6125, 6125, '0', '0', 0, 4023.15966796875, 1907.605712890625, 353.23004150390625, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 1, 46366), -- Titan Energy Ring (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+30, 213946, 1008, 6125, 6125, '0', '0', 0, 4023.063720703125, 1876.00830078125, 181.318328857421875, 0, -0.6839151382446289, 0, 0, 0.729561567306518554, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+31, 213528, 1008, 6125, 6125, '0', '0', 0, 4023.15966796875, 1907.605712890625, 300.169921875, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 1, 46366), -- Titan Energy Ring (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+32, 213949, 1008, 6125, 6125, '0', '0', 0, 4052.369140625, 1907.501953125, 137.716705322265625, 1.570795059204101562, -0.48360109329223632, -0.48360061645507812, 0.515877723693847656, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+33, 213948, 1008, 6125, 6125, '0', '0', 0, 4023.255859375, 1939.1881103515625, 181.318328857421875, 3.143104076385498046, 0, 0.683915138244628906, -0.72956085205078125, 0.001035800902172923, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+34, 213951, 1008, 6125, 6125, '0', '0', 0, 3993.950439453125, 1907.6944580078125, 137.7166748046875, 4.712390422821044921, -0.48360109329223632, 0.483600616455078125, -0.51587772369384765, 0.51587843894958496, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+35, 213950, 1008, 6125, 6125, '0', '0', 0, 4023.0634765625, 1878.3887939453125, 137.716705322265625, 0, -0.6839151382446289, 0, 0, 0.729561567306518554, 7200, 255, 1, 46366), -- Mogu Energy Rune (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+36, 213373, 1008, 6125, 6125, '0', '0', 0, 4227.2705078125, 1560.15771484375, 423.96978759765625, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 0, 46366), -- Mysterious Wall (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+37, 213506, 1008, 6125, 6125, '0', '0', 0, 4022.8388671875, 1908.992431640625, 358.941497802734375, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 0, 46366), -- Titan Energy Discs (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+38, 214497, 1008, 6125, 6125, '0', '0', 0, 3899.1728515625, 1140.7760009765625, 481.12890625, 4.711977005004882812, 0, 0, -0.70725250244140625, 0.706961035728454589, 7200, 255, 0, 46366), -- Stone Guard Entrance Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+39, 211311, 1008, 6125, 6125, '0', '0', 0, 3870.75, 1246.280029296875, 466.280487060546875, 4.54347848892211914, 0, 0, -0.76423454284667968, 0.644938409328460693, 7200, 255, 1, 46366), -- Quilin Rubble (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+40, 211626, 1008, 6125, 6125, '0', '0', 0, 4027.302978515625, 1331.392333984375, 466.3052978515625, 5.678255081176757812, 0, 0, -0.29787445068359375, 0.954605042934417724, 7200, 255, 0, 46366), -- Nullification Barrier (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+41, 211628, 1008, 6125, 6125, '0', '0', 0, 4028.325439453125, 1352.8541259765625, 468.80902099609375, 5.25200653076171875, 0, 0, -0.49304771423339843, 0.870002269744873046, 7200, 255, 0, 46366), -- Shroud of Reversal (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+42, 213933, 1008, 6125, 6125, '0', '0', 0, 4236.15087890625, 1341.326416015625, 454.157958984375, 0, 0, 0, 0, 1, 7200, 255, 0, 46366), -- Mogu_Raised_Gate01 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+43, 213933, 1008, 6125, 6125, '0', '0', 0, 4276.79443359375, 1382.71875, 454.158050537109375, 4.712389945983886718, 0, 0, -0.70710659027099609, 0.707106947898864746, 7200, 255, 0, 46366), -- Mogu_Raised_Gate01 (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+44, 215114, 1008, 6125, 6125, '0', '0', 0, 4262.08154296875, 1577.38720703125, 438.803924560546875, 2.747625350952148437, 0, 0, 0.980661392211914062, 0.195712178945541381, 7200, 255, 1, 46366), -- Wind Wall (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+45, 215114, 1008, 6125, 6125, '0', '0', 0, 4192.5146484375, 1577.404541015625, 438.803955078125, 0.334916114807128906, 0, 0, 0.166676521301269531, 0.986011624336242675, 7200, 255, 1, 46366), -- Wind Wall (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+46, 214412, 1008, 6125, 6125, '0', '0', 0, 4023.778564453125, 1834.6163330078125, 358.788330078125, 1.566573500633239746, 0, 0, 0.7056121826171875, 0.708598196506500244, 7200, 255, 0, 46366), -- Celestial Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+47, 211650, 1008, 6125, 6125, '0', '0', 0, 4023.130126953125, 1863.5999755859375, 358.788299560546875, 4.708658218383789062, 0, 0, -0.70842456817626953, 0.705786526203155517, 7200, 255, 1, 46366), -- Celestial Control Console (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+48, 213526, 1008, 6125, 6125, '0', '0', 0, 4023.15966796875, 1907.6036376953125, 360.077484130859375, 3.141592741012573242, 0, 0, -1, 0, 7200, 255, 1, 46366), -- Energy Platform (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+49, 214383, 1008, 6125, 6125, '0', '0', 0, 4023.145751953125, 1844.6805419921875, 358.788330078125, 4.704121112823486328, 0, 0, -0.71002388000488281, 0.704177558422088623, 7200, 255, 0, 46366), -- Cache of Pure Energy (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+50, 211584, 1008, 6125, 6125, '0', '0', 0, 3942.39404296875, 1550.267333984375, 369.56146240234375, 0.017538294196128845, 0, 0, 0.008769035339355468, 0.999961555004119873, 7200, 255, 0, 46366), -- Ancient Control Console (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+51, 211603, 1008, 6125, 6125, '0', '0', 0, 3934.335205078125, 1575.0625, 362.313323974609375, 3.121866703033447265, 0, 0, 0.999951362609863281, 0.009862797334790229, 7200, 255, 0, 46366), -- Terracotta Army Door (Area: Mogu'shan Vaults - Difficulty: 3)
(@OGUID+52, 211603, 1008, 6125, 6125, '0', '0', 0, 3934.407958984375, 1524.986083984375, 362.313201904296875, 3.124342918395996093, 0, 0, 0.999962806701660156, 0.008624766021966934, 7200, 255, 0, 46366); -- Terracotta Army Door (Area: Mogu'shan Vaults - Difficulty: 3)

DELETE FROM gameobject_addon WHERE guid BETWEEN @OGUID+0 AND @OGUID+52;
INSERT INTO gameobject_addon (guid, parent_rotation0, parent_rotation1, parent_rotation2, parent_rotation3, WorldEffectID, AIAnimKitID) VALUES
(@OGUID+1, 0, 0, 0.998134791851043701, 0.061048660427331924, 0, 0), -- Inner Doors
(@OGUID+2, 0, 0, 0.998134791851043701, 0.061048660427331924, 0, 0), -- Outer Doors
(@OGUID+3, 0, 0, 1, -0.00000004371138828, 0, 0), -- Ancient Mogu Door
(@OGUID+5, 0, 0, 1, -0.00000004371138828, 0, 0), -- Ancient Mogu Door
(@OGUID+6, 0, 0, 1, -0.00000004371138828, 0, 0), -- Doodad_Mogu_Statue_MoguShanVaults003
(@OGUID+8, 0, 0, 1, -0.00000004371138828, 0, 0), -- Doodad_Mogu_Statue_MoguShanVaults002
(@OGUID+9, 0, 0, 1, -0.00000004371138828, 0, 0), -- Ancient Mogu Door
(@OGUID+10, 0, 0, 1, -0.00000004371138828, 0, 0), -- Ancient Mogu Door
(@OGUID+11, 0, 0, 1, -0.00000004371138828, 0, 0), -- Ancient Mogu Door
(@OGUID+12, 0, 0, 1, -0.00000004371138828, 0, 0), -- Doodad_Mogu_Statue_MoguShanVaults004
(@OGUID+13, 0, 0, 1, -0.00000004371138828, 0, 0), -- Doodad_Mogu_Statue_MoguShanVaults001
(@OGUID+14, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+15, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+16, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+17, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+18, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+19, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+20, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+21, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+22, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+23, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+24, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+25, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+26, 0, 0, 1, -0.00000004371138828, 0, 0), -- Titan Energy Ring
(@OGUID+27, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+28, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+29, 0, 0, 1, -0.00000004371138828, 0, 0), -- Titan Energy Ring
(@OGUID+30, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+31, 0, 0, 1, -0.00000004371138828, 0, 0), -- Titan Energy Ring
(@OGUID+32, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+33, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+34, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+35, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mogu Energy Rune
(@OGUID+36, 0, 0, 1, -0.00000004371138828, 0, 0), -- Mysterious Wall
(@OGUID+37, 0, 0, 1, -0.00000004371138828, 0, 0), -- Titan Energy Discs
(@OGUID+48, 0, 0, 1, -0.00000004371138828, 0, 0); -- Energy Platform

DELETE FROM creature_template_addon WHERE entry IN (67221 /67221 (Terracotta Boss) - Ride Vehicle Hardcoded/, 60575 /60575 (Terracotta Boss) - Ride Vehicle Hardcoded/, 60399 /60399 (Qin-xi) - Magnetic Armor/, 60400 /60400 (Jan-xi) - Magnetic Armor/, 60768 /60768 (Emperor's Strength) - Ride Vehicle Hardcoded/, 60648 /60648 (Ancient Mogu Machine)/, 60396 /60396 (Emperor's Rage) - Unarmored, Terracotta Stone Cosmetic/, 60913 /60913 (Energy Charge) - Core Checker/, 60410 /60410 (Elegon) - Elegon Spawn Cosmetic/, 65293 /65293 (Infinite Energy) - Spawn Flash 1 Periodic/, 61330 /61330 (Energy Vortex Stalker) - Charge Vortex/, 60960 /60960 (Core Target Stalker)/, 64068 /64068 (Mogu'shan Engine Keeper) - Spirit Particles (Spawn)/, 64063 /64063 (Mogu'shan Arcanist) - Spirit Particles (Spawn)/, 60731 /60731 (Undying Shadows) - Undying Shadows/, 60958 /60958 (Pinning Arrow) - Pinning Arrow/, 60942 /60942 (Volley)/, 60847 /60847 (Flanking Mogu) - Ghostly Visual/, 64947 /64947 (Mogu'shan Warden) - Spirit Particles (Spawn), Mogu Patrol Cosmetic/, 64061 /64061 (Mogu'shan Warden) - Spirit Particles (Spawn), Mogu Patrol Cosmetic/, 65297 /65297 (Invisible Stalker)/, 60708 /60708 (Meng the Demented) - Energy Drain/, 60701 /60701 (Zian of the Endless Shadow) - Inactive Visual/, 60984 /60984 (Controller)/, 60710 /60710 (Subetai the Swift) - Inactive Visual/, 60709 /60709 (Qiang the Merciless) - Massive Attacks/, 61505 /61505 (Volley)/, 60776 /60776 (Empyreal Focus) - Focus State Cosmetic - Initial Inactive State/, 59677 /59677 (General Purpose Bunny JMF (Look 2, Flying)) - Water Cosmetic/, 61423 /61423 (Qiang the Merciless) - Inactive Visual/, 61421 /61421 (Zian of the Endless Shadow) - Inactive Visual/, 61427 /61427 (Subetai the Swift) - Inactive Visual/, 61250 /61250 (Sorcerer Mogu) - Inactive Visual/, 61341 /61341 (Mounted Mogu) - Inactive Visual/, 61347 /61347 (Kingsguard) - Inactive Visual/, 61429 /61429 (Meng the Demented) - Inactive Visual/, 61345 /61345 (Mogu Archer) - Inactive Visual/, 65343 /65343 (Bone Spider)/, 61172 /61172 (Gate to the Spirit World) - Spirit Portal/, 61140 /61140 (Gara'jal the Spiritbinder) - Ghost Visual/, 60240 /60240 (Spirit Totem) - Conduit to the Spirit Realm/, 60184 /60184 (Shadowy Minion) - Shadowy, Shadowy Minion Cosmetic Spell/, 60940 /60940 (Shadowy Minion) - Shadowy Minion Cosmetic Spell/, 60387 /60387 (Troll Explosives) - Troll Explosives/, 60394 /60394 (Troll Explosives) - Explosion/, 60395 /60395 (Troll Explosives) - Explosion/, 60392 /60392 (Troll Explosives) - Explosion/, 60393 /60393 (Troll Explosives) - Explosion/, 60390 /60390 (Troll Explosives) - Explosion/, 60391 /60391 (Troll Explosives) - Explosion/, 60388 /60388 (Troll Explosives) - Explosion/, 60389 /60389 (Troll Explosives) - Explosion/, 65228 /65228 (Mogu Runes (Pitch Forward 180) DNT) - Mogu Runes (Pitch Forward 180) DNT/, 64526 /64526 (Ancient Mogu Artifact) - Achievement Verification Aura/, 64529 /64529 (Mogu Runes (Horizontal Visual)) - Mogu Runes (Horizontal)/, 65227 /65227 (Mogu Runes (Pitch Forward 120) DNT) - Mogu Runes (Pitch Forward 120) DNT/, 64600 /64600 (Mogu Runes (Vertical Visual)) - Mogu Runes (Vertical)/, 61139 /61139 (Shan-xi Watershaper) - Spirit Particles (Spawn), Frost Channelling/, 61131 /61131 (Mogu'shan Secret-Keeper) - Spirit Particles (Spawn)/, 61352 /61352 (Story Time)/, 60375 /60375 (Zandalari Skullcharger) - Mod Scale 95-100%, Zandalarian Big Troll, Mod Scale 95-100%/, 60402 /60402 (Zandalari Fire-Dancer) - Fire Channeling/, 60381 /60381 (Zandalari Infiltrator) - Mod Scale 95-100%/, 60644 /60644 (Troll Explosives) - Troll Explosives/, 60438 /60438 (Wildfire Spark) - Archimonde Fire/, 60384 /60384 (Zandalari Pterror Wing) - Mod Scale 95-100%/, 60386 /60386 (Zandalari Terror Rider) - Mod Scale 95-100%, Mount Up, Quick to React/, 60559 /60559 (Zandalari Spirit)/, 60143 /60143 (Gara'jal the Spiritbinder) - Mighty Mojo/, 60241 /60241 (Lightning Charge) - Lightning Fists/, 61134 /61134 (Lightning Charge)/, 60385 /60385 (Zandalari War Wyvern)/, 60558 /60558 (Zandalari Spirit)/, 60513 /60513 (Spirit Totem)/, 60512 /60512 (Spirit Totem)/, 60009 /60009 (Feng the Accursed) - Unarmed/, 62213 /62213 (Feng Attachment Point) - Ride Vehicle Hardcoded/, 61124 /61124 (Phase Controller) - Phase Transition (Staff)/, 60397 /60397 (Emperor's Strength) - Full Plate, Terracotta Stone Cosmetic/, 60398 /60398 (Emperor's Courage) - Half Plate, Terracotta Stone Cosmetic/, 61989 /61989 (Cursed Mogu Sculpture) - Stone, Pose/, 62004 /62004 (Ghost Essence) - Ghost Essence/, 61334 /61334 (Cursed Mogu Sculpture) - Bronze, Pose/, 43499 /43499 (Consecration)/, 59915 /59915 (Jasper Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit/, 60047 /60047 (Amethyst Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit/, 60043 /60043 (Jade Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit/, 60089 /60089 (Stone Guard Controller) - Dampening Field/, 64825 /64825 (Leng Windstaff)/, 61348 /61348 (Lorewalker Cho)/, 64183 /64183 (Enormous Stone Quilen) - Dampening Field/, 60576 /60576 (Stone Quilen) - Dampening Field, Stone Form/);
INSERT INTO creature_template_addon (entry, path_id, mount, bytes1, bytes2, emote, aiAnimKit, movementAnimKit, meleeAnimKit, visibilityDistanceType, auras) VALUES
(67221, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '46598'), -- 67221 (Terracotta Boss) - Ride Vehicle Hardcoded
(60575, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '46598'), -- 60575 (Terracotta Boss) - Ride Vehicle Hardcoded
(60399, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116815'), -- 60399 (Qin-xi) - Magnetic Armor
(60400, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117193'), -- 60400 (Jan-xi) - Magnetic Armor
(60768, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '46598'), -- 60768 (Emperor's Strength) - Ride Vehicle Hardcoded
(60648, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- 60648 (Ancient Mogu Machine)
(60396, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116535 118039'), -- 60396 (Emperor's Rage) - Unarmored, Terracotta Stone Cosmetic
(60913, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '118024'), -- 60913 (Energy Charge) - Core Checker
(60410, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127808'), -- 60410 (Elegon) - Elegon Spawn Cosmetic
(65293, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, '127785'), -- 65293 (Infinite Energy) - Spawn Flash 1 Periodic
(61330, 0, 0, 0, 1, 0, 0, 0, 0, 4, '118983'), -- 61330 (Energy Vortex Stalker) - Charge Vortex
(60960, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 60960 (Core Target Stalker)
(64068, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- 64068 (Mogu'shan Engine Keeper) - Spirit Particles (Spawn)
(64063, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- 64063 (Mogu'shan Arcanist) - Spirit Particles (Spawn)
(60731, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117514'), -- 60731 (Undying Shadows) - Undying Shadows
(60958, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118141'), -- 60958 (Pinning Arrow) - Pinning Arrow
(60942, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 60942 (Volley)
(60847, 0, 0, 50331648, 1, 0, 2178, 2179, 0, 3, '117904'), -- 60847 (Flanking Mogu) - Ghostly Visual
(64947, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585 128807'), -- 64947 (Mogu'shan Warden) - Spirit Particles (Spawn), Mogu Patrol Cosmetic
(64061, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585 128807'), -- 64061 (Mogu'shan Warden) - Spirit Particles (Spawn), Mogu Patrol Cosmetic
(65297, 0, 0, 50331648, 1, 0, 0, 0, 0, 4, ''), -- 65297 (Invisible Stalker)
(60708, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117707'), -- 60708 (Meng the Demented) - Energy Drain
(60701, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118205'), -- 60701 (Zian of the Endless Shadow) - Inactive Visual
(60984, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 60984 (Controller)
(60710, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118205'), -- 60710 (Subetai the Swift) - Inactive Visual
(60709, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117920'), -- 60709 (Qiang the Merciless) - Massive Attacks
(61505, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 61505 (Volley)
(60776, 0, 0, 0, 1, 0, 0, 0, 0, 4, '127303'), -- 60776 (Empyreal Focus) - Focus State Cosmetic - Initial Inactive State
(59677, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '118595'), -- 59677 (General Purpose Bunny JMF (Look 2, Flying)) - Water Cosmetic
(61423, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- 61423 (Qiang the Merciless) - Inactive Visual
(61421, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- 61421 (Zian of the Endless Shadow) - Inactive Visual
(61427, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- 61427 (Subetai the Swift) - Inactive Visual
(61250, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- 61250 (Sorcerer Mogu) - Inactive Visual
(61341, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- 61341 (Mounted Mogu) - Inactive Visual
(61347, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- 61347 (Kingsguard) - Inactive Visual
(61429, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '119310'), -- 61429 (Meng the Demented) - Inactive Visual
(61345, 0, 0, 0, 1, 0, 0, 0, 0, 3, '119310'), -- 61345 (Mogu Archer) - Inactive Visual
(65343, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 65343 (Bone Spider)
(61172, 0, 0, 33554432, 1, 0, 0, 0, 0, 0, '118661'), -- 61172 (Gate to the Spirit World) - Spirit Portal
(61140, 0, 0, 33554432, 1, 0, 0, 0, 0, 3, '91218'), -- 61140 (Gara'jal the Spiritbinder) - Ghost Visual
(60240, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116827'), -- 60240 (Spirit Totem) - Conduit to the Spirit Realm
(60184, 0, 0, 0, 1, 0, 0, 0, 0, 0, '120767 118086'), -- 60184 (Shadowy Minion) - Shadowy, Shadowy Minion Cosmetic Spell
(60940, 0, 0, 0, 1, 0, 0, 0, 0, 3, '118086'), -- 60940 (Shadowy Minion) - Shadowy Minion Cosmetic Spell
(60387, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116545'), -- 60387 (Troll Explosives) - Troll Explosives
(60394, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60394 (Troll Explosives) - Explosion
(60395, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60395 (Troll Explosives) - Explosion
(60392, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60392 (Troll Explosives) - Explosion
(60393, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60393 (Troll Explosives) - Explosion
(60390, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60390 (Troll Explosives) - Explosion
(60391, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60391 (Troll Explosives) - Explosion
(60388, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60388 (Troll Explosives) - Explosion
(60389, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116508'), -- 60389 (Troll Explosives) - Explosion
(65228, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127567'), -- 65228 (Mogu Runes (Pitch Forward 180) DNT) - Mogu Runes (Pitch Forward 180) DNT
(64526, 0, 0, 0, 1, 0, 0, 0, 0, 0, '126029'), -- 64526 (Ancient Mogu Artifact) - Achievement Verification Aura
(64529, 0, 0, 0, 1, 0, 0, 0, 0, 0, '126038'), -- 64529 (Mogu Runes (Horizontal Visual)) - Mogu Runes (Horizontal)
(65227, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '127566'), -- 65227 (Mogu Runes (Pitch Forward 120) DNT) - Mogu Runes (Pitch Forward 120) DNT
(64600, 0, 0, 50331648, 1, 0, 0, 0, 0, 0, '126183'), -- 64600 (Mogu Runes (Vertical Visual)) - Mogu Runes (Vertical)
(61139, 0, 0, 0, 1, 0, 0, 0, 0, 3, '37119 45846'), -- 61139 (Shan-xi Watershaper) - Spirit Particles (Spawn), Frost Channelling
(61131, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123585'), -- 61131 (Mogu'shan Secret-Keeper) - Spirit Particles (Spawn)
(61352, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- 61352 (Story Time)
(60375, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167 116966 120379'), -- 60375 (Zandalari Skullcharger) - Mod Scale 95-100%, Zandalarian Big Troll, Mod Scale 95-100%
(60402, 0, 0, 0, 1, 0, 0, 0, 0, 3, '117826'), -- 60402 (Zandalari Fire-Dancer) - Fire Channeling
(60381, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167'), -- 60381 (Zandalari Infiltrator) - Mod Scale 95-100%
(60644, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116545'), -- 60644 (Troll Explosives) - Troll Explosives
(60438, 0, 0, 0, 1, 0, 0, 0, 0, 0, '116787'), -- 60438 (Wildfire Spark) - Archimonde Fire
(60384, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, '123167'), -- 60384 (Zandalari Pterror Wing) - Mod Scale 95-100%
(60386, 0, 0, 0, 1, 0, 0, 0, 0, 3, '123167 116548 116600'), -- 60386 (Zandalari Terror Rider) - Mod Scale 95-100%, Mount Up, Quick to React
(60559, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 60559 (Zandalari Spirit)
(60143, 0, 0, 0, 1, 0, 0, 0, 0, 3, '115964'), -- 60143 (Gara'jal the Spiritbinder) - Mighty Mojo
(60241, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116225'), -- 60241 (Lightning Charge) - Lightning Fists
(61134, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 61134 (Lightning Charge)
(60385, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- 60385 (Zandalari War Wyvern)
(60558, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 60558 (Zandalari Spirit)
(60513, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 60513 (Spirit Totem)
(60512, 0, 0, 50331648, 1, 0, 0, 0, 0, 3, ''), -- 60512 (Spirit Totem)
(60009, 0, 0, 0, 1, 0, 0, 0, 0, 3, '124252'), -- 60009 (Feng the Accursed) - Unarmed
(62213, 0, 0, 0, 1, 0, 0, 0, 0, 3, '46598'), -- 62213 (Feng Attachment Point) - Ride Vehicle Hardcoded
(61124, 0, 0, 0, 1, 0, 0, 0, 0, 0, '118486'), -- 61124 (Phase Controller) - Phase Transition (Staff)
(60397, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116540 118039'), -- 60397 (Emperor's Strength) - Full Plate, Terracotta Stone Cosmetic
(60398, 0, 0, 0, 1, 0, 0, 0, 0, 3, '116537 118039'), -- 60398 (Emperor's Courage) - Half Plate, Terracotta Stone Cosmetic
(61989, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120663 120650'), -- 61989 (Cursed Mogu Sculpture) - Stone, Pose
(62004, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120681'), -- 62004 (Ghost Essence) - Ghost Essence
(61334, 0, 0, 0, 1, 0, 0, 0, 0, 3, '120661 120613'), -- 61334 (Cursed Mogu Sculpture) - Bronze, Pose
(43499, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 43499 (Consecration)
(59915, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- 59915 (Jasper Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit
(60047, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- 60047 (Amethyst Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit
(60043, 0, 0, 0, 1, 0, 0, 0, 0, 3, '72242 115745 128886'), -- 60043 (Jade Guardian) - Zero Energy + Zero Regen, Solid Stone, Quilen Freeze Sit Anim Kit
(60089, 0, 0, 0, 1, 0, 0, 0, 0, 4, '126816'), -- 60089 (Stone Guard Controller) - Dampening Field
(64825, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 64825 (Leng Windstaff)
(61348, 0, 0, 0, 1, 0, 0, 0, 0, 3, ''), -- 61348 (Lorewalker Cho)
(64183, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816'), -- 64183 (Enormous Stone Quilen) - Dampening Field
(60576, 0, 0, 0, 1, 0, 0, 0, 0, 3, '126816 117838'), -- 60576 (Stone Quilen) - Dampening Field; Stone Form

UPDATE creature_template_addon SET visibilityDistanceType=3 WHERE entry=59481; -- 59481 (World Trigger)
UPDATE creature_template_addon SET auras='' WHERE entry=55091; -- 55091 (General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI))

DELETE FROM creature_template_scaling WHERE (DifficultyID=3 AND Entry IN (67221,60575,60399,60400,60768,60648,60396,60913,60410,65293,61330,60960,64068,64063,60731,60958,60942,60847,64947,64061,65297,60708,60701,60984,60710,60709,61505,60776,59677,61423,61421,61427,61250,61341,61347,61429,61345,65343,61172,61140,60240,60184,60940,60387,60394,60395,60392,60393,60390,60391,60388,60389,65228,64526,64529,65227,64600,61139,61131,61352,60375,60402,60381,60644,60438,60384,60386,60559,60143,60241,61134,60385,60558,60513,60512,60009,62213,61124,60397,60398,61989,62004,61334,43499,59481,54020,59915,60047,60043,60089,55091,36737,64825,61348,64183,60576));
INSERT INTO creature_template_scaling (Entry, DifficultyID, LevelScalingDeltaMin, LevelScalingDeltaMax, ContentTuningID, VerifiedBuild) VALUES
(67221, 3, 3, 3, 1032, 46366),
(60575, 3, 3, 3, 1032, 46366),
(60399, 3, 3, 3, 1032, 46366),
(60400, 3, 3, 3, 1032, 46366),
(60768, 3, 2, 2, 1032, 46366),
(60648, 3, 3, 3, 1032, 46366),
(60396, 3, 2, 2, 1032, 46366),
(60913, 3, 0, 0, 1032, 46366),
(60410, 3, 3, 3, 1032, 46366),
(65293, 3, 0, 0, 1032, 46366),
(61330, 3, 0, 0, 1032, 46366),
(60960, 3, 0, 0, 1032, 46366),
(64068, 3, 2, 2, 1032, 46366),
(64063, 3, 2, 2, 1032, 46366),
(60731, 3, 2, 2, 1032, 46366),
(60958, 3, 2, 2, 1032, 46366),
(60942, 3, 2, 2, 1032, 46366),
(60847, 3, 3, 3, 1032, 46366),
(64947, 3, 2, 2, 1032, 46366),
(64061, 3, 2, 2, 1032, 46366),
(65297, 3, 0, 0, 1032, 46366),
(60708, 3, 3, 3, 1032, 46366),
(60701, 3, 3, 3, 1032, 46366),
(60984, 3, 2, 2, 1032, 46366),
(60710, 3, 3, 3, 1032, 46366),
(60709, 3, 3, 3, 1032, 46366),
(61505, 3, 2, 2, 1032, 46366),
(60776, 3, 0, 0, 1032, 46366),
(59677, 3, 0, 0, 328, 46366),
(61423, 3, 3, 3, 1032, 46366),
(61421, 3, 3, 3, 1032, 46366),
(61427, 3, 3, 3, 1032, 46366),
(61250, 3, 2, 2, 1032, 46366),
(61341, 3, 2, 2, 1032, 46366),
(61347, 3, 2, 2, 1032, 46366),
(61429, 3, 3, 3, 1032, 46366),
(61345, 3, 2, 2, 1032, 46366),
(65343, 3, 0, 0, 81, 46366),
(61172, 3, 2, 2, 1032, 46366),
(61140, 3, 3, 3, 1032, 46366),
(60240, 3, 2, 2, 1032, 46366),
(60184, 3, 2, 2, 1032, 46366),
(60940, 3, 2, 2, 1032, 46366),
(60387, 3, 2, 2, 1032, 46366),
(60394, 3, 2, 2, 1032, 46366),
(60395, 3, 2, 2, 1032, 46366),
(60392, 3, 2, 2, 1032, 46366),
(60393, 3, 2, 2, 1032, 46366),
(60390, 3, 2, 2, 1032, 46366),
(60391, 3, 2, 2, 1032, 46366),
(60388, 3, 2, 2, 1032, 46366),
(60389, 3, 2, 2, 1032, 46366),
(65228, 3, 3, 3, 1032, 46366),
(64526, 3, 3, 3, 1032, 46366),
(64529, 3, 3, 3, 1032, 46366),
(65227, 3, 3, 3, 1032, 46366),
(64600, 3, 3, 3, 1032, 46366),
(61139, 3, 0, 0, 1032, 46366),
(61131, 3, 2, 2, 1032, 46366),
(61352, 3, 2, 2, 1032, 46366),
(60375, 3, 2, 2, 1032, 46366),
(60402, 3, 2, 2, 1032, 46366),
(60381, 3, 2, 2, 1032, 46366),
(60644, 3, 2, 2, 1032, 46366),
(60438, 3, 3, 3, 1032, 46366),
(60384, 3, 2, 2, 1032, 46366),
(60386, 3, 2, 2, 1032, 46366),
(60559, 3, 2, 2, 1032, 46366),
(60143, 3, 3, 3, 1032, 46366),
(60241, 3, 3, 3, 1032, 46366),
(61134, 3, 3, 3, 1032, 46366),
(60385, 3, 2, 2, 1032, 46366),
(60558, 3, 2, 2, 1032, 46366),
(60513, 3, 2, 2, 1032, 46366),
(60512, 3, 2, 2, 1032, 46366),
(60009, 3, 3, 3, 1032, 46366),
(62213, 3, 0, 0, 1032, 46366),
(61124, 3, 3, 3, 1032, 46366),
(60397, 3, 2, 2, 1032, 46366),
(60398, 3, 2, 2, 1032, 46366),
(61989, 3, 0, 0, 1032, 46366),
(62004, 3, 0, 0, 1032, 46366),
(61334, 3, 0, 0, 1032, 46366),
(43499, 3, 0, 0, 482, 46366),
(59481, 3, 0, 0, 181, 46366),
(54020, 3, 0, 0, 328, 46366),
(59915, 3, 3, 3, 1032, 46366),
(60047, 3, 3, 3, 1032, 46366),
(60043, 3, 3, 3, 1032, 46366),
(60089, 3, 3, 3, 1032, 46366),
(55091, 3, 0, 0, 328, 46366),
(36737, 3, 0, 0, 328, 46366),
(64825, 3, 1, 1, 1032, 46366),
(61348, 3, 2, 2, 1032, 46366),
(64183, 3, 1, 1, 1032, 46366),
(60576, 3, 1, 1, 1032, 46366);

UPDATE creature_model_info SET BoundingRadius=6.651214599609375, CombatReach=8.75, VerifiedBuild=46366 WHERE DisplayID IN (41391, 41392);
UPDATE creature_model_info SET BoundingRadius=2.850520610809326171, CombatReach=4.5, VerifiedBuild=46366 WHERE DisplayID=41388;
UPDATE creature_model_info SET BoundingRadius=0.310000002384185791, VerifiedBuild=46366 WHERE DisplayID=41689;
UPDATE creature_model_info SET BoundingRadius=16.67825508117675781, CombatReach=13.75, VerifiedBuild=46366 WHERE DisplayID=41399;
UPDATE creature_model_info SET BoundingRadius=2, CombatReach=2, VerifiedBuild=46366 WHERE DisplayID=41571;
UPDATE creature_model_info SET BoundingRadius=3, CombatReach=3, VerifiedBuild=46366 WHERE DisplayID=41718;
UPDATE creature_model_info SET BoundingRadius=3, CombatReach=7.5, VerifiedBuild=46366 WHERE DisplayID=43367;
UPDATE creature_model_info SET BoundingRadius=1.73610997200012207, CombatReach=7.5, VerifiedBuild=46366 WHERE DisplayID=42429;
UPDATE creature_model_info SET BoundingRadius=0.347222000360488891, CombatReach=1.5, VerifiedBuild=46366 WHERE DisplayID=41286;
UPDATE creature_model_info SET BoundingRadius=3.800693988800048828, CombatReach=6, VerifiedBuild=46366 WHERE DisplayID=41193;
UPDATE creature_model_info SET BoundingRadius=0.5, CombatReach=1, VerifiedBuild=46366 WHERE DisplayID=31379;

DELETE FROM npc_text WHERE ID BETWEEN @NPCTEXTID+0 AND @NPCTEXTID+3;
INSERT INTO npc_text (ID, Probability0, Probability1, Probability2, Probability3, Probability4, Probability5, Probability6, Probability7, BroadcastTextId0, BroadcastTextId1, BroadcastTextId2, BroadcastTextId3, BroadcastTextId4, BroadcastTextId5, BroadcastTextId6, BroadcastTextId7, VerifiedBuild) VALUES
(@NPCTEXTID+0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 46366), -- 64825 (Leng Windstaff)
(@NPCTEXTID+1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 46366), -- 64825 (Leng Windstaff)
(@NPCTEXTID+2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 46366), -- 61348 (Lorewalker Cho)
(@NPCTEXTID+3, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 46366); -- 61348 (Lorewalker Cho)

DELETE FROM gossip_menu WHERE (MenuID=14602 AND TextID=@NPCTEXTID+0) OR (MenuID=14603 AND TextID=@NPCTEXTID+1) OR (MenuID=14622 AND TextID=@NPCTEXTID+2) OR (MenuID=13757 AND TextID=@NPCTEXTID+3);
INSERT INTO gossip_menu (MenuID, TextID, VerifiedBuild) VALUES
(14602, @NPCTEXTID+0, 46366), -- 64825 (Leng Windstaff)
(14603, @NPCTEXTID+1, 46366), -- 64825 (Leng Windstaff)
(14622, @NPCTEXTID+2, 46366), -- 61348 (Lorewalker Cho)
(13757, @NPCTEXTID+3, 46366); -- 61348 (Lorewalker Cho)

DELETE FROM gossip_menu WHERE (MenuID=14604 AND TextID=1);
INSERT INTO gossip_menu (MenuID, TextID, VerifiedBuild) VALUES
(14604, 1, 46366); -- 64825 (Leng Windstaff)

UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=1 WHERE entry IN (67221, 60575); -- Terracotta Boss
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags=32832 WHERE entry=60399; -- Qin-xi
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags=64 WHERE entry=60400; -- Jan-xi
UPDATE creature_template SET minlevel=37, maxlevel=37, unit_flags3=1 WHERE entry=60768; -- Emperor's Strength
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=60648; -- Ancient Mogu Machine
UPDATE creature_template SET minlevel=37, maxlevel=37, speed_walk=1, unit_flags3=1 WHERE entry=60396; -- Emperor's Rage
UPDATE creature_template SET minlevel=35, maxlevel=35, faction=14, speed_walk=0.919999957084655761, speed_run=0.394285708665847778, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=4194304, unit_flags3=1, HoverHeight=2 WHERE entry=60913; -- Energy Charge
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags2=4227072 WHERE entry=60410; -- Elegon
UPDATE creature_template SET minlevel=35, maxlevel=35, speed_walk=2.40000009536743164, speed_run=1.285714268684387207, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=4196352, unit_flags3=524288 WHERE entry=65293; -- Infinite Energy
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=61330; -- Energy Vortex Stalker
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=60960; -- Core Target Stalker
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=64068; -- Mogu'shan Engine Keeper
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=64063; -- Mogu'shan Arcanist
UPDATE creature_template SET minlevel=37, maxlevel=37, speed_run=0.571428596973419189, BaseAttackTime=1500, unit_flags2=2099200, unit_flags3=524288 WHERE entry=60731; -- Undying Shadows
UPDATE creature_template SET minlevel=37, maxlevel=37, unit_flags3=1 WHERE entry=60958; -- Pinning Arrow
UPDATE creature_template SET minlevel=37, maxlevel=37, speed_walk=2, speed_run=0.714285731315612792, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=2099200, unit_flags3=524289 WHERE entry IN (60942, 61505); -- Volley
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=60847; -- Flanking Mogu
UPDATE creature_template SET minlevel=37, maxlevel=37, dynamicflags=0 WHERE entry IN (64947, 64061); -- Mogu'shan Warden
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=65297; -- Invisible Stalker
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry IN (60708, 61429); -- Meng the Demented
UPDATE creature_template SET minlevel=38, maxlevel=38, dynamicflags=0 WHERE entry=60701; -- Zian of the Endless Shadow
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60984; -- Controller
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry IN (60710, 61427); -- Subetai the Swift
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry IN (60709, 61423); -- Qiang the Merciless
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=60776; -- Empyreal Focus
UPDATE creature_template SET minlevel=60, maxlevel=60 WHERE entry=59677; -- General Purpose Bunny JMF (Look 2, Flying)
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=61421; -- Zian of the Endless Shadow
UPDATE creature_template SET minlevel=37, maxlevel=37, dynamicflags=0 WHERE entry=61250; -- Sorcerer Mogu
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=61341; -- Mounted Mogu
UPDATE creature_template SET minlevel=37, maxlevel=37, dynamicflags=0 WHERE entry=61347; -- Kingsguard
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=61345; -- Mogu Archer
UPDATE creature_template SET minlevel=37, maxlevel=37, BaseAttackTime=2000, unit_flags=256, unit_flags2=67110912, unit_flags3=17301504, VehicleId=2180 WHERE entry=61172; -- Gate to the Spirit World
UPDATE creature_template SET minlevel=38, maxlevel=38, speed_walk=3.20000004768371582, speed_run=2, BaseAttackTime=2000, unit_flags=256, unit_flags2=2099200, unit_flags3=524288, HoverHeight=8 WHERE entry=61140; -- Gara'jal the Spiritbinder
UPDATE creature_template SET minlevel=37, maxlevel=37, speed_run=1, BaseAttackTime=2000, unit_flags2=2129920, unit_flags3=524289 WHERE entry=60240; -- Spirit Totem
UPDATE creature_template SET minlevel=37, maxlevel=37, BaseAttackTime=2000, unit_flags3=524288, dynamicflags=1 WHERE entry=60184; -- Shadowy Minion
UPDATE creature_template SET minlevel=37, maxlevel=37, BaseAttackTime=2000, unit_flags=256, unit_flags2=69208066, unit_flags3=17301505 WHERE entry=60940; -- Shadowy Minion
UPDATE creature_template SET minlevel=37, maxlevel=37, BaseAttackTime=2000, unit_flags=256, unit_flags2=2099200, unit_flags3=524289 WHERE entry=60387; -- Troll Explosives
UPDATE creature_template SET minlevel=37, maxlevel=37, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=2048, unit_flags3=524289 WHERE entry IN (60394, 60395, 60392, 60393, 60390, 60391, 60388, 60389); -- Troll Explosives
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=65228; -- Mogu Runes (Pitch Forward 180) DNT
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=64526; -- Ancient Mogu Artifact
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=64529; -- Mogu Runes (Horizontal Visual)
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=65227; -- Mogu Runes (Pitch Forward 120) DNT
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=64600; -- Mogu Runes (Vertical Visual)
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=61139; -- Shan-xi Watershaper
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=61131; -- Mogu'shan Secret-Keeper
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=61352; -- Story Time
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60375; -- Zandalari Skullcharger
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60402; -- Zandalari Fire-Dancer
UPDATE creature_template SET minlevel=37, maxlevel=37, dynamicflags=0 WHERE entry=60381; -- Zandalari Infiltrator
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60644; -- Troll Explosives
UPDATE creature_template SET minlevel=38, maxlevel=38, speed_walk=1.60000002384185791, speed_run=0.571428596973419189, BaseAttackTime=2000, unit_flags=33554496, unit_flags2=2048, unit_flags3=524289 WHERE entry=60438; -- Wildfire Spark
UPDATE creature_template SET minlevel=37, maxlevel=37, HoverHeight=1.08899998664855957 WHERE entry=60384; -- Zandalari Pterror Wing
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60386; -- Zandalari Terror Rider
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry IN (60559, 60558); -- Zandalari Spirit
UPDATE creature_template SET minlevel=38, maxlevel=38, dynamicflags=0 WHERE entry=60143; -- Gara'jal the Spiritbinder
UPDATE creature_template SET minlevel=38, maxlevel=38, speed_walk=3.20000004768371582, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=2623488, unit_flags3=524289 WHERE entry=60241; -- Lightning Charge
UPDATE creature_template SET minlevel=38, maxlevel=38, speed_walk=4, speed_run=1.428571462631225585, BaseAttackTime=2000, unit_flags=33554432, unit_flags2=2623488, unit_flags3=524289 WHERE entry=61134; -- Lightning Charge
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60385; -- Zandalari War Wyvern
UPDATE creature_template SET minlevel=37, maxlevel=37, unit_flags3=16777217 WHERE entry IN (60513, 60512); -- Spirit Totem
UPDATE creature_template SET minlevel=38, maxlevel=38, dynamicflags=0 WHERE entry=60009; -- Feng the Accursed
UPDATE creature_template SET minlevel=35, maxlevel=35, unit_flags3=16777217 WHERE entry=62213; -- Feng Attachment Point
UPDATE creature_template SET minlevel=38, maxlevel=38, unit_flags3=16777217 WHERE entry=61124; -- Phase Controller
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60397; -- Emperor's Strength
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=60398; -- Emperor's Courage
UPDATE creature_template SET minlevel=35, maxlevel=35 WHERE entry=61989; -- Cursed Mogu Sculpture
UPDATE creature_template SET minlevel=35, maxlevel=35, unit_flags3=16777217 WHERE entry=62004; -- Ghost Essence
UPDATE creature_template SET minlevel=35, maxlevel=35, dynamicflags=0 WHERE entry=61334; -- Cursed Mogu Sculpture
UPDATE creature_template SET minlevel=51, maxlevel=51, npcflag=16777216, BaseAttackTime=1000000, unit_flags=33554432, unit_flags2=32, unit_flags3=1 WHERE entry=43499; -- Consecration
UPDATE creature_template SET unit_flags3=16777217 WHERE entry=59481; -- World Trigger
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=59915; -- Jasper Guardian
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=60047; -- Amethyst Guardian
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=60043; -- Jade Guardian
UPDATE creature_template SET minlevel=38, maxlevel=38 WHERE entry=60089; -- Stone Guard Controller
UPDATE creature_template SET minlevel=60, maxlevel=60 WHERE entry=36737; -- Invisible Stalker
UPDATE creature_template SET minlevel=36, maxlevel=36 WHERE entry=64825; -- Leng Windstaff
UPDATE creature_template SET minlevel=37, maxlevel=37 WHERE entry=61348; -- Lorewalker Cho
UPDATE creature_template SET minlevel=36, maxlevel=36 WHERE entry=64183; -- Enormous Stone Quilen
UPDATE creature_template SET minlevel=36, maxlevel=36, unit_flags=32768 WHERE entry=60576; -- Stone Quilen

DELETE FROM creature_text WHERE (CreatureID IN(59915, 60009, 60043, 60047, 60143, 60410, 60576, 60648, 60701, 60708, 60709, 60710, 61140, 61348, 62004, 64068));
INSERT INTO creature_text (CreatureID, GroupID, ID, Text, Type, Language, Probability, Emote, Duration, Sound, BroadcastTextId, TextRange, comment) VALUES
(59915, 0, 0, '%s is about to Overload!', 41, 0, 100, 0, 0, 0, 61726, 0, 'Jasper Guardian'),
(59915, 1, 0, '|TInterface\Icons\inv_misc_qirajicrystal_02.blp:20|t |c00F19C88%s|r begins to petrify all players!', 41, 0, 100, 0, 0, 0, 59700, 0, 'Jasper Guardian to Stone Guard Controller'),
(60009, 0, 0, 'Tender your souls, mortals! These are the halls of the dead!', 14, 0, 100, 0, 0, 27814, 61068, 0, 'Feng the Accursed'),
(60009, 1, 0, 'Oh great spirit! Grant me the power of the earth!', 14, 0, 100, 0, 0, 27816, 61096, 0, 'Feng the Accursed to Phase Controller'),
(60009, 2, 0, '%s begins to channel a violent |cFFFF0404|Hspell:116040|h[Epicenter]|h|r!', 41, 0, 100, 0, 0, 0, 59714, 0, 'Feng the Accursed to Feng the Accursed'),
(60009, 3, 0, 'Oh exalted one! Through me you shall melt flesh from bone!', 14, 0, 100, 0, 0, 27820, 61097, 0, 'Feng the Accursed to Phase Controller'),
(60009, 4, 0, '%s begins to |cFFFF0404|Hspell:116711|h[Draw Flame]|h|r to his weapon!', 41, 0, 100, 0, 0, 0, 59886, 0, 'Feng the Accursed'),
(60009, 5, 0, 'You have been affected by |cFFFF0404|Hspell:116784|h[Wildfire Spark]|h|r !', 42, 0, 100, 0, 0, 0, 61728, 0, 'Feng the Accursed to Player'),
(60009, 6, 0, 'Oh sage of the ages! Instill to me your arcane wisdom!', 14, 0, 100, 0, 0, 27821, 61098, 0, 'Feng the Accursed to Phase Controller'),
(60009, 7, 0, 'If they only knew... what horrors await...', 14, 0, 100, 0, 0, 27815, 61100, 0, 'Feng the Accursed to Player'),
(60043, 0, 0, '|TInterface\Icons\inv_misc_qirajicrystal_03.blp:20|t |c0093F8D1%s|r begins to petrify all players!', 41, 0, 100, 0, 0, 0, 59699, 0, 'Jade Guardian to Stone Guard Controller'),
(60043, 1, 0, '%s is about to Overload!', 41, 0, 100, 0, 0, 0, 61726, 0, 'Jade Guardian'),
(60047, 0, 0, '%s is about to Overload!', 41, 0, 100, 0, 0, 0, 61726, 0, 'Amethyst Guardian'),
(60143, 0, 0, 'A pair of Zandalari Trolls burst through the doors ahead of you!', 41, 0, 100, 0, 0, 0, 59870, 0, 'Gara'jal the Spiritbinder to Zandalari Skullcharger'),
(60143, 1, 0, 'No matter! We don't be needin' what's back dat way anyhow!', 14, 0, 100, 0, 0, 29086, 59871, 0, 'Gara'jal the Spiritbinder to Zandalari Infiltrator'),
(60143, 2, 0, 'Now you done made me angry!', 14, 0, 100, 0, 0, 29087, 59872, 0, 'Gara'jal the Spiritbinder to Zandalari Pterror Wing'),
(60143, 3, 0, 'Fine, den! Time ta play, friends.', 14, 0, 100, 0, 0, 29088, 59873, 0, 'Gara'jal the Spiritbinder to Zandalari Skullcharger'),
(60143, 4, 0, 'It be dyin' time, now!', 14, 0, 100, 0, 0, 29080, 63317, 0, 'Gara'jal the Spiritbinder'),
(60143, 5, 0, '|TInterface\Icons\inv_misc_idol_02.blp:20|tGara'jal selects random players to become |cFFFF0000|Hspell:122151|h[Voodoo Dolls]|h|r!', 41, 0, 100, 0, 0, 0, 65370, 0, 'Gara'jal the Spiritbinder'),
(60143, 6, 0, '|TInterface\Icons\spell_nature_agitatingtotem.blp:20|tGara'jal summons a Spirit Totem. Use it to |cFFFF0000|Hspell:116161|h[Cross Over]|h|r to the Spirit World!', 41, 0, 100, 0, 0, 0, 66277, 0, 'Gara'jal the Spiritbinder'),
(60143, 7, 0, '|TInterface\Icons\ability_druid_challangingroar.blp:20|tGara'jal casts |cFFFF0000|Hspell:117752|h[Frenzy]|h|r!', 41, 0, 100, 0, 0, 0, 65382, 0, 'Gara'jal the Spiritbinder'),
(60410, 0, 0, 'Sensors online. All cores operating at peak efficiency.', 14, 0, 100, 0, 0, 28544, 65996, 0, 'Elegon to Invisible Stalker'),
(60410, 1, 0, 'Identification matrix mismatch. Unknown entities detected.', 14, 0, 100, 0, 0, 28545, 65998, 0, 'Elegon to Invisible Stalker'),
(60410, 2, 0, 'Entering defensive mode. Disabling output failsafes.', 14, 0, 100, 0, 0, 28534, 60158, 0, 'Elegon'),
(60410, 3, 0, 'Drawing from reserve power.', 14, 0, 100, 0, 0, 28538, 63825, 0, 'Elegon'),
(60410, 4, 0, 'Existence ends... I become nothing...', 14, 0, 100, 0, 0, 28535, 63835, 0, 'Elegon to Player'),
(60576, 0, 0, '%s goes into a |cFFFF0000|Hspell:116982|h[Protective Frenzy]|h|r!', 41, 0, 100, 0, 0, 0, 59989, 0, 'Stone Quilen'),
(60648, 0, 0, 'The machine hums to life! Get to the lower level!', 41, 0, 100, 0, 0, 0, 60536, 0, 'Ancient Mogu Machine to Player'),
(60648, 1, 0, 'The Emperor's Rage echoes through the hills.', 14, 0, 100, 0, 0, 29018, 61487, 0, 'Ancient Mogu Machine'),
(60648, 2, 0, 'The Emperor's Strength grips this land with fists of iron.', 14, 0, 100, 0, 0, 29019, 61488, 0, 'Ancient Mogu Machine'),
(60648, 3, 0, 'The Emperor's Strength appears in the alcoves!', 41, 0, 100, 0, 0, 0, 66158, 0, 'Ancient Mogu Machine'),
(60648, 4, 0, 'The Emperor's Courage burns everlasting.', 14, 0, 100, 0, 0, 29020, 61489, 0, 'Ancient Mogu Machine'),
(60648, 5, 0, 'The Emperor's Courage appears in the alcoves!', 41, 0, 100, 0, 0, 0, 66160, 0, 'Ancient Mogu Machine'),
(60648, 6, 0, 'The Endless Army will crush the Emperor's foes.', 14, 0, 100, 0, 0, 29017, 61486, 0, 'Ancient Mogu Machine'),
(60648, 7, 0, 'Two titanic constructs appear in the large alcoves!', 41, 0, 100, 0, 0, 0, 66347, 0, 'Ancient Mogu Machine'),
(60701, 0, 0, 'Soon you will understand why my subjects fear the shadows!', 14, 0, 100, 0, 0, 27958, 62976, 0, 'Zian of the Endless Shadow to Player'),
(60701, 1, 0, '|TInterface\Icons\ability_rogue_envelopingshadows.blp:20|t%s casts |cFFFF0000|Hspell:117506|h[Undying Shadows]|h|r!', 41, 0, 100, 0, 0, 27964, 60537, 0, 'Zian of the Endless Shadow'),
(60701, 2, 0, 'The darkness comes for you, and with it, death.', 14, 0, 100, 0, 0, 27964, 62981, 0, 'Zian of the Endless Shadow'),
(60708, 0, 0, 'You have angered the emperor! THE SENTENCE IS DEATH!', 14, 0, 100, 0, 0, 28069, 62947, 0, 'Meng the Demented to Player'),
(60708, 1, 0, '|TInterface\Icons\warrior_talent_icon_furyintheblood:20|t%s casts |cFFFF0000|Hspell:117708|h[Maddening Shout]|h|r!', 41, 0, 100, 0, 0, 28077, 60538, 0, 'Meng the Demented'),
(60708, 2, 0, 'Slaughter yourselves for my amusement!', 14, 0, 100, 0, 0, 28077, 62954, 0, 'Meng the Demented'),
(60708, 3, 0, 'I... I don't want to die!', 14, 0, 100, 0, 0, 28071, 62948, 0, 'Meng the Demented to Meng the Demented'),
(60709, 0, 0, 'My armies are endless, my power, unmatched.', 14, 0, 100, 0, 0, 28059, 62956, 0, 'Qiang the Merciless'),
(60709, 1, 0, 'I will crush you, in body AND spirit.', 14, 0, 100, 0, 0, 28057, 62959, 0, 'Qiang the Merciless to Player'),
(60709, 2, 0, '|TInterface\Icons\ability_warrior_stalwartprotector.blp:20|t%s casts |cFFFF0000|Hspell:117910|h[Flanking Orders]|h|r!', 41, 0, 100, 0, 0, 28063, 60292, 0, 'Qiang the Merciless'),
(60709, 3, 0, 'Soldiers! Crush their flank!', 14, 0, 100, 0, 0, 28063, 62965, 0, 'Qiang the Merciless'),
(60709, 4, 0, 'Flanking attack! March into battle!', 14, 0, 100, 0, 0, 28064, 62966, 0, 'Qiang the Merciless'),
(60710, 0, 0, 'You'll see your mistake soon enough!', 14, 0, 100, 0, 0, 28109, 62968, 0, 'Subetai the Swift to Player'),
(61140, 0, 0, 'Bah! Bested by de likes of you! What a shame for a Zandalari Priest ta endure.', 14, 0, 100, 15, 0, 29082, 60653, 0, 'Gara'jal the Spiritbinder to Gara'jal the Spiritbinder'),
(61140, 1, 0, 'But it ain't no ting. Take your trinkets; you'll be needin' dem.', 14, 0, 100, 14, 0, 29083, 60654, 0, 'Gara'jal the Spiritbinder to Gara'jal the Spiritbinder'),
(61140, 2, 0, 'I be goin' now; time ta find a new home for me soul.', 14, 0, 100, 1, 0, 29084, 60655, 0, 'Gara'jal the Spiritbinder to Gara'jal the Spiritbinder'),
(61140, 3, 0, 'Death be a doorway, an' time a window; I'll be back! AH-HAHAHAHAHAH!', 14, 0, 100, 0, 0, 29085, 60656, 0, 'Gara'jal the Spiritbinder to Gara'jal the Spiritbinder'),
(61348, 0, 0, 'The ancient vaults of the mogu, untouched by time for thousands of years, who knows what secrets await?', 12, 0, 100, 1, 0, 30198, 65168, 0, 'Lorewalker Cho to Player'),
(61348, 1, 0, 'Many young Pandaren warriors have vanished into this...tomb. Have your wits about you.', 12, 0, 100, 1, 0, 30199, 65170, 0, 'Lorewalker Cho to Player'),
(61348, 2, 0, 'The mogu held considerable power over the stones of the earth. Their magic lingers on, even centuries later.', 12, 0, 100, 1, 0, 30178, 65171, 0, 'Lorewalker Cho to Stone Quilen'),
(61348, 3, 0, 'The large Quilen seem to be guarding that door. Tread carefully, lest you wake them.', 12, 0, 100, 25, 0, 30179, 65172, 0, 'Lorewalker Cho to Enormous Stone Quilen'),
(61348, 4, 0, 'That was close!', 14, 0, 100, 5, 0, 30180, PLEASE_SET_A_BROADCASTTEXT_ID, 0, 'Lorewalker Cho to Amethyst Guardian'), -- BroadcastTextID: 46780 - 65173
(61348, 5, 0, 'The door mechanism seems to have activated with the demise of the guardians. I fear what we may find within.', 12, 0, 100, 1, 0, 30181, 65174, 0, 'Lorewalker Cho to Amethyst Guardian'),
(61348, 6, 0, 'We seem to have awakened a spirit of some kind.', 12, 0, 100, 6, 0, 30193, 65175, 0, 'Lorewalker Cho to Cursed Mogu Sculpture'),
(61348, 7, 0, 'So the stories were true...', 12, 0, 100, 274, 0, 30194, 65176, 0, 'Lorewalker Cho to Player'),
(61348, 8, 0, 'We have disturbed what should not have been disturbed. He cannot be free to escape this prison! You must destroy him!', 12, 0, 100, 5, 0, 30195, 65177, 0, 'Lorewalker Cho to Player'),
(61348, 9, 0, 'Feng the Accursed was betrayed by four emperors of the mogu, his body destroyed each time, only to rise again.', 12, 0, 100, 1, 0, 30196, 65178, 0, 'Lorewalker Cho to Player'),
(61348, 10, 0, 'This is not the last we have seen of Feng...', 12, 0, 100, 274, 0, 30197, 65180, 0, 'Lorewalker Cho to Player'),
(61348, 11, 0, 'The Zandalari, ancient allies of the mogu! They must seek the legendary weapon within the depths of the Vaults!', 12, 0, 100, 5, 0, 30209, 65181, 0, 'Lorewalker Cho to Player'),
(61348, 12, 0, 'I am sorry I did not tell you earlier! These vaults house a weapon of unimaginable power! Stop them!', 12, 0, 100, 1, 0, 30208, 65188, 0, 'Lorewalker Cho to Player'),
(61348, 13, 0, 'I will stay behind and investigate this dais further...', 12, 0, 100, 2, 0, 30210, 65189, 0, 'Lorewalker Cho to Player'),
(61348, 14, 0, 'Ah, the ancient Vaults of the Kings of Mogu'shan. What might this button do...?', 12, 0, 100, 1, 0, 30176, 60833, 0, 'Lorewalker Cho to Player'),
(61348, 15, 0, 'Look here! It's the history of Zian of the Endless Shadow, a powerful sorcerer who kept the kingdoms in check through terror and darkness.', 12, 0, 100, 1, 0, 30207, 60728, 0, 'Lorewalker Cho to Player'),
(61348, 16, 0, 'This looks to be the record of Qiang the Merciless, a great conqueror who ruled his vassals with an iron fist!', 12, 0, 100, 1, 0, 31529, 60857, 0, 'Lorewalker Cho to Sorcerer Mogu'),
(61348, 17, 0, 'I've heard tales of this one: Subetai the Swift, well known as a masterful archer and genius thief. He stole from the rich, and kept everything for himself.', 12, 0, 100, 1, 0, 30177, 60861, 0, 'Lorewalker Cho to Mounted Mogu'),
(61348, 18, 0, 'What is this? I've heard nothing of this emperor. Meng the Demented? According to these texts, Meng was the most foul and maniacal ruler to ever abuse the imperial throne.', 12, 0, 100, 275, 0, 31528, 60865, 0, 'Lorewalker Cho to Mogu Archer'),
(61348, 19, 0, 'A secret passage has opened beneath the platform, this way!', 14, 0, 100, 5, 0, 30200, 65190, 0, 'Lorewalker Cho to Player'),
(61348, 20, 0, 'Look at the dust along the floor, these dark halls have not seen footfalls for many centuries.', 12, 0, 100, 1, 0, 30201, 65191, 0, 'Lorewalker Cho to Player'),
(61348, 21, 0, 'Once, the mogu enslaved all of the races of Pandaria, and bent them to their nefarious will.', 12, 0, 100, 25, 0, 30202, 65192, 0, 'Lorewalker Cho to Player'),
(61348, 22, 0, 'These Jinyu have been locked in an eternity of servitude.', 12, 0, 100, 1, 0, 30203, 65193, 0, 'Lorewalker Cho to Player'),
(61348, 23, 0, 'I hear a faint humming. It is coming from this direction!', 12, 0, 100, 25, 0, 30204, 65194, 0, 'Lorewalker Cho to Mogu'shan Secret-Keeper'),
(61348, 24, 0, 'This room pulses with energy, but feels unlike any we have seen so far.', 12, 0, 100, 5, 0, 30182, 65991, 0, 'Lorewalker Cho to Mogu'shan Warden'),
(61348, 25, 0, 'Fascinating, I have never heard tales of anything like this. The mogu hide their secrets well.', 12, 0, 100, 5, 0, 30183, 65992, 0, 'Lorewalker Cho to Mogu'shan Warden'),
(61348, 26, 0, 'This seems to be a terminal controlling the device. Let's see here...', 12, 0, 100, 25, 0, 30184, 65993, 0, 'Lorewalker Cho to Player'),
(61348, 27, 0, 'Magnificent! It talks! This seems beyond what we thought the mogu were capable of.', 12, 0, 100, 5, 0, 30185, 66000, 0, 'Lorewalker Cho to Invisible Stalker'),
(61348, 28, 0, 'Oh dear...this could get messy. I will observe from a safer distance!', 12, 0, 100, 274, 0, 30186, 66001, 0, 'Lorewalker Cho to Invisible Stalker'),
(61348, 29, 0, 'The machine has been destroyed, and so the mogu's power weakens.', 12, 0, 100, 4, 0, 30187, 66005, 0, 'Lorewalker Cho to Player'),
(61348, 30, 0, 'These discs look familiar, but I cannot place their origin.', 12, 0, 100, 25, 0, 30188, 66007, 0, 'Lorewalker Cho to Player'),
(61348, 31, 0, 'They call to me, the discs...I must...', 12, 0, 100, 6, 0, 30189, 66008, 0, 'Lorewalker Cho to Player'),
(61348, 32, 0, 'I...I see everything clearly now.', 12, 0, 100, 2, 0, 30190, 66009, 0, 'Lorewalker Cho to Player'),
(61348, 33, 0, 'The Titans! It would seem the mogu salvaged and repurposed ancient Titan technology to advance their empire!', 12, 0, 100, 5, 0, 30191, 66010, 0, 'Lorewalker Cho to Player'),
(61348, 34, 0, 'We must go, we are not done yet.', 12, 0, 100, 1, 0, 30192, 66011, 0, 'Lorewalker Cho to Player'),
(61348, 35, 0, 'May the great crane guide your steps.', 12, 0, 100, 66, 0, 31814, 65202, 0, 'Lorewalker Cho to Player'),
(61348, 36, 0, 'It is done, then. Yet...how many of these machines are hidden beneath the earth? How much do we not know about the dark secrets of the mogu?', 12, 0, 100, 4, 0, 30205, 65195, 0, 'Lorewalker Cho to Player'),
(61348, 37, 0, 'I fear that our war against our ancient enemies is still far from over.', 12, 0, 100, 1, 0, 30206, 65196, 0, 'Lorewalker Cho to Player'),
(62004, 0, 0, 'Your presence disturbs a lingering spirit!', 41, 0, 100, 0, 0, 0, 61566, 0, 'Ghost Essence to Cursed Mogu Sculpture'),
(64068, 0, 0, 'This is the closest any outsider has come to the Engine of Nalak'sha.', 14, 0, 100, 0, 0, 0, 65930, 0, 'Mogu'shan Engine Keeper to Mogu'shan Warden'),
(64068, 1, 0, 'These secrets will die with you.', 14, 0, 100, 0, 0, 0, 65940, 0, 'Mogu'shan Engine Keeper to Mogu'shan Warden');

DELETE FROM vehicle_template_accessory WHERE (entry IN(60397, 60399, 60400, 60512, 60384, 62004) AND accessory_entry IN (60768, 67221, 60575, 60513, 60386, 62213));
INSERT INTO vehicle_template_accessory (entry, accessory_entry, seat_id, minion, description, summontype, summontimer) VALUES
(60397, 60768, 0, 0, 'Emperor's Strength - Emperor's Strength', 6, 30000), -- Emperor's Strength - Emperor's Strength
(60399, 67221, 2, 0, 'Qin-xi - Terracotta Boss', 6, 30000), -- Qin-xi - Terracotta Boss
(60399, 67221, 1, 0, 'Qin-xi - Terracotta Boss', 6, 30000), -- Qin-xi - Terracotta Boss
(60399, 67221, 0, 0, 'Qin-xi - Terracotta Boss', 6, 30000), -- Qin-xi - Terracotta Boss
(60400, 60575, 2, 0, 'Jan-xi - Terracotta Boss', 6, 30000), -- Jan-xi - Terracotta Boss
(60400, 60575, 1, 0, 'Jan-xi - Terracotta Boss', 6, 30000), -- Jan-xi - Terracotta Boss
(60400, 60575, 0, 0, 'Jan-xi - Terracotta Boss', 6, 30000), -- Jan-xi - Terracotta Boss
(60512, 60513, 0, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(60384, 60386, 0, 0, 'Zandalari Pterror Wing - Zandalari Terror Rider', 6, 30000), -- Zandalari Pterror Wing - Zandalari Terror Rider
(60512, 60513, 6, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(60512, 60513, 7, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(60512, 60513, 1, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(60512, 60513, 4, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(60512, 60513, 3, 0, 'Spirit Totem - Spirit Totem', 6, 30000), -- Spirit Totem - Spirit Totem
(62004, 62213, 4, 0, 'Ghost Essence - Feng Attachment Point', 6, 30000), -- Ghost Essence - Feng Attachment Point
(62004, 62213, 3, 0, 'Ghost Essence - Feng Attachment Point', 6, 30000), -- Ghost Essence - Feng Attachment Point
(62004, 62213, 2, 0, 'Ghost Essence - Feng Attachment Point', 6, 30000), -- Ghost Essence - Feng Attachment Point
(62004, 62213, 1, 0, 'Ghost Essence - Feng Attachment Point', 6, 30000), -- Ghost Essence - Feng Attachment Point
(62004, 62213, 0, 0, 'Ghost Essence - Feng Attachment Point', 6, 30000); -- Ghost Essence - Feng Attachment Point

DELETE FROM npc_spellclick_spells WHERE (npc_entry IN(60644, 64600, 65227, 64529, 64526,65228) AND spell_id IN (117956, 126022));
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type) VALUES
(60644, 117956, 0, 1), -- Disarm Trap Spell (Rogue)
(64600, 126022, 0, 1), -- Activate Artifact
(65227, 126022, 0, 1), -- Activate Artifact
(64529, 126022, 0, 1), -- Activate Artifact
(64526, 126022, 0, 1), -- Activate Artifact
(65228, 126022, 0, 1); -- Activate Artifact

UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=14505; -- Dreadsteed
UPDATE creature_template SET HealthModifier=0.800000011920928955, CreatureDifficultyID=57312, VerifiedBuild=46366 WHERE entry=60184; -- Shadowy Minion
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=43499; -- Consecration
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=59677; -- General Purpose Bunny JMF (Look 2, Flying)
UPDATE creature_template SET HealthModifier=0.60000002384185791, CreatureDifficultyID=57315, VerifiedBuild=46366 WHERE entry=60240; -- Spirit Totem
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=65343; -- Bone Spider
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=21354; -- Fiery Warhorse
UPDATE creature_template SET CreatureDifficultyID=55776, VerifiedBuild=46366 WHERE entry=60375; -- Zandalari Skullcharger
UPDATE creature_template SET CreatureDifficultyID=57390, VerifiedBuild=46366 WHERE entry=60384; -- Zandalari Pterror Wing
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=304; -- Felsteed
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=305; -- White Stallion
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=32640; -- Traveler's Tundra Mammoth
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=59481; -- World Trigger
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=28302; -- Acherus Deathcharger
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=55091; -- General Purpose Bunny JMF (Look 2 - Flying, Infinite AOI)
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=36737; -- Invisible Stalker
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=20029; -- Thalassian Warhorse
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=20030; -- Thalassian Charger
UPDATE creature_template SET HealthModifier=270, CreatureDifficultyID=56801, VerifiedBuild=46366 WHERE entry=60143; -- Gara'jal the Spiritbinder
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=54020; -- General Purpose Bunny JMF (Look 2 - Flying, Huge AOI)
UPDATE creature_template SET HealthScalingExpansion=9, VerifiedBuild=46366 WHERE entry=18377; -- Swift Red Wind Rider

UPDATE gameobject_template SET castBarCaption='Activating', Data3=0, Data14=27700, ContentTuningId=1032, VerifiedBuild=46366 WHERE entry=211584; -- Ancient Control Console
UPDATE gameobject_template SET Data1=0, Data10=1, Data11=1, Data12=1, Data13=1, Data20=90, Data25=1500, Data30=90053, ContentTuningId=1032, VerifiedBuild=46366 WHERE entry=214383; -- Cache of Pure Energy
UPDATE gameobject_template SET VerifiedBuild=46366 WHERE entry IN (213526, 214412, 215114, 213933, 211311, 214497, 213955, 213954, 213953, 213952, 213951, 213950, 213949, 213948, 213947, 213946, 213945, 213944, 213943, 213942, 213941, 213940, 213939, 213938, 213937, 213529, 213528, 213527, 213506, 213373, 213244, 213285, 210868, 211693, 213258, 214696, 213247, 213248, 213246, 214452, 213245, 214526, 214441);
UPDATE gameobject_template SET ContentTuningId=1032, VerifiedBuild=46366 WHERE entry=211650; -- Celestial Control Console
UPDATE gameobject_template SET ContentTuningId=1032, VerifiedBuild=46366 WHERE entry=211628; -- Shroud of Reversal
UPDATE gameobject_template SET ContentTuningId=1032, VerifiedBuild=46366 WHERE entry=211626; -- Nullification Barrier