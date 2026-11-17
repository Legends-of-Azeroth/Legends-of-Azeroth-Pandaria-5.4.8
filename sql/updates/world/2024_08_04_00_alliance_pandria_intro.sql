-- Alliance Pandria intro quest 
-- The Mission (29548) 
-- Phase 1686
-- Remove old creature 570539 - 570611
DELETE FROM `creature` WHERE `guid` BETWEEN 570539 AND 570611;
 
SET @CGUID:=4000000;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+77;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `PhaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `dynamicflags`, `VerifiedBuild`) VALUES
(@CGUID+0, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7873.548828125, 1287.454833984375, 341.7664794921875, 1.134044289588928222, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+1, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.12158203125, 1339.751708984375, 338.94195556640625, 6.279649734497070312, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+2, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7896.697265625, 1353.548095703125, 338.84442138671875, 4.525454998016357421, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+3, 64865, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7884.00537109375, 1329.3385009765625, 338.943206787109375, 0.207194462418556213, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Airman Rogers (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+4, 40789, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7880.4501953125, 1331.9300537109375, 340.028350830078125, 1.56033480167388916, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Generic Controller Bunny (CSA) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 132223 - FX_AllianceGunship_Idle_Main_Loop)
(@CGUID+5, 66292, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7879.80029296875, 1279.515625, 358.560516357421875, 4.728822231292724609, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Sky Admiral Rogers (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+6, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7872.8037109375, 1304.171875, 339.23492431640625, 4.737676620483398437, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+7, 64861, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7887.7099609375, 1286.3350830078125, 341.642608642578125, 1.766041874885559082, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Captain Day (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+8, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.13720703125, 1327.7274169921875, 338.945404052734375, 0.113753728568553924, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+9, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7893.82275390625, 1352.9947509765625, 338.914794921875, 1.610516309738159179, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+10, 64866, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7878.921875, 1280.26220703125, 358.559600830078125, 4.733302116394042968, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Corporal Rasmussen (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+11, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.251953125, 1349.09033203125, 338.946044921875, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+12, 55825, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.6787109375, 1263.5260009765625, 358.571624755859375, 2.055117130279541015, 120, 0, 0, 2854, 100, 0, 0, 0, 0, 0, 55261), -- Amber Kearnen (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 131904 - Kearnen's Sniper Rifle)
(@CGUID+13, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7921.798828125, 1273.421875, 343.420989990234375, 3.069559097290039062, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+14, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7894.1494140625, 1317.888916015625, 338.965118408203125, 2.206909894943237304, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+15, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.53125, 1349.0885009765625, 338.94482421875, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+16, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7894.0380859375, 1341.6475830078125, 338.965118408203125, 2.206909894943237304, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+17, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7872.69091796875, 1282.875, 358.55609130859375, 3.080678701400756835, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+18, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.32666015625, 1351.1146240234375, 338.94525146484375, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+19, 55832, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7882.61962890625, 1265.5086669921875, 358.571319580078125, 3.663313865661621093, 120, 0, 0, 2854, 100, 0, 0, 0, 0, 0, 55261), -- Nodd Codejack (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+20, 55819, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7886.45166015625, 1278.298583984375, 358.559478759765625, 5.19431304931640625, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Sully "The Pickle" McLeary (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+21, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.35400390625, 1319.4410400390625, 338.943389892578125, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+22, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.67041015625, 1346.9114990234375, 338.939697265625, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+23, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7867.330078125, 1341.9444580078125, 338.965118408203125, 0.826461434364318847, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+24, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7924.33154296875, 1276.03125, 343.4189453125, 3.148760318756103515, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+25, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.47900390625, 1347.0225830078125, 338.9453125, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+26, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.38525390625, 1346.9114990234375, 338.94091796875, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+27, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7872.0625, 1261.82470703125, 358.56201171875, 3.193963050842285156, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+28, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.173828125, 1317.5225830078125, 338.954833984375, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+29, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.15087890625, 1319.5555419921875, 338.954254150390625, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+30, 32520, 0, 1519, 6555, 1, 1686, 0, 0, 0, -8290.09375, 1388.640625, 30.0042724609375, 0, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Totally Generic Bunny (All Phase) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 - !!! already present in database !!!
(@CGUID+31, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.376953125, 1348.9444580078125, 338.94097900390625, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+32, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.41162109375, 1351.078125, 338.941009521484375, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+33, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.21875, 1315.35595703125, 338.9473876953125, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+34, 64863, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7880.923828125, 1280.2430419921875, 358.5596923828125, 4.738608360290527343, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Sky Mage Harlan (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+35, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.29541015625, 1317.3802490234375, 338.945159912109375, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+36, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.1943359375, 1347.032958984375, 338.946533203125, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+37, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.72900390625, 1330.123291015625, 338.940185546875, 3.065017223358154296, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+38, 55370, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7881.20849609375, 1328.6510009765625, 350.846588134765625, 0, 120, 0, 0, 112919, 3155, 0, 0, 0, 0, 0, 55261), -- General Purpose Bunny ZTO (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 130860 - Skyfire Defense Patrols)
(@CGUID+39, 40789, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7843.47998046875, 1265.739990234375, 308.835357666015625, 5.26434946060180664, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Generic Controller Bunny (CSA) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+40, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.78125, 1328.0816650390625, 338.941009521484375, 2.986098051071166992, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+41, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7892.84716796875, 1356.23095703125, 338.885986328125, 3.191249370574951171, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+42, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.5537109375, 1330.1285400390625, 338.9423828125, 0.188897475600242614, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+43, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.71875, 1340.4132080078125, 338.938934326171875, 3.058433771133422851, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+44, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.26220703125, 1342.1822509765625, 338.943603515625, 0.071608141064643859, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+45, 66460, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.81591796875, 1342.451416015625, 338.938934326171875, 3.137352943420410156, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter Pilot (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+46, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7894.15478515625, 1327.890625, 338.965118408203125, 2.206909894943237304, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+47, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7866.8037109375, 1318.0885009765625, 338.965118408203125, 0.826461434364318847, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+48, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7888.03466796875, 1261.607666015625, 358.561767578125, 0.088901109993457794, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+49, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.6474609375, 1348.9427490234375, 338.939727783203125, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+50, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.43408203125, 1319.5538330078125, 338.9517822265625, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+51, 64858, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.5869140625, 1329.173583984375, 338.940948486328125, 2.97155928611755371, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Airman Hicks (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+52, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7887.3681640625, 1282.5538330078125, 358.55633544921875, 6.190460205078125, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+53, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7925.9130859375, 1276.001708984375, 343.511077880859375, 0.126102268695831298, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+54, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7893.859375, 1306.3072509765625, 338.965118408203125, 2.206909894943237304, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+55, 64862, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.44970703125, 1340.9635009765625, 338.9415283203125, 0.084910094738006591, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Airman Ellis (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+56, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.6943359375, 1351.07470703125, 338.93975830078125, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+57, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7867.07275390625, 1328.107666015625, 338.965118408203125, 0.826461434364318847, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+58, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.60791015625, 1351.111083984375, 338.944122314453125, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+59, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.578125, 1317.3802490234375, 338.94268798828125, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+60, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.63525390625, 1319.435791015625, 338.94091796875, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+61, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7875.501953125, 1315.3524169921875, 338.944915771484375, 3.16120314598083496, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+62, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.4150390625, 1315.388916015625, 338.9522705078125, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+63, 55823, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7884.51220703125, 1266.4322509765625, 358.568023681640625, 5.011447429656982421, 120, 0, 0, 2854, 100, 0, 0, 0, 0, 0, 55261), -- Mishka (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+64, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7835.6005859375, 1276.99658203125, 343.510009765625, 3.143929243087768554, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+65, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7883.44970703125, 1317.5208740234375, 338.952301025390625, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+66, 66473, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7866.55908203125, 1306.532958984375, 338.965118408203125, 0.826461434364318847, 120, 0, 0, 28540, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Gyrocopter (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+67, 64860, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7871.9775390625, 1286.13720703125, 341.750457763671875, 2.807392597198486328, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Captain Kerwin (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+68, 66300, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.1337890625, 1315.392333984375, 338.95477294921875, 0.018319351598620414, 120, 0, 0, 8562, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Marine (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+69, 64864, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7877.4443359375, 1341.3697509765625, 338.93927001953125, 3.084580659866333007, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Airman Clifton (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+70, 64859, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7880.6240234375, 1353.932861328125, 338.838165283203125, 1.572238922119140625, 120, 0, 0, 14270, 0, 0, 0, 0, 0, 0, 55261), -- Sky Captain Callanan (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+71, 32520, 0, 1519, 6555, 1, 1686, 0, 0, 0, -8680.671875, 730.748291015625, 159.0923919677734375, 0, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Totally Generic Bunny (All Phase) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 - !!! already present in database !!!
(@CGUID+72, 40789, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7915.58984375, 1261.06005859375, 316.726348876953125, 2.032872200012207031, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Generic Controller Bunny (CSA) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+73, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7867.08984375, 1356.173095703125, 338.838043212890625, 5.118679046630859375, 120, 10, 0, 2854, 0, 1, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+74, 66480, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7872.3662109375, 1303.05908203125, 339.27850341796875, 2.036499977111816406, 120, 0, 0, 2854, 0, 0, 0, 0, 0, 0, 55261), -- Skyfire Engineer (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+75, 55830, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7885.5712890625, 1276.6336669921875, 358.560791015625, 2.30356454849243164, 120, 0, 0, 2854, 100, 0, 0, 0, 0, 0, 55261), -- Nimm Codejack (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@CGUID+76, 40789, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7917.60986328125, 1389.77001953125, 302.996337890625, 2.980149745941162109, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261), -- Generic Controller Bunny (CSA) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+77, 40789, 0, 1519, 6555, 1, 1686, 0, 0, 0, -7842.080078125, 1389.6600341796875, 301.831329345703125, 2.753256797790527343, 120, 0, 0, 112919, 0, 0, 0, 0, 0, 0, 55261); -- Generic Controller Bunny (CSA) (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)

DELETE FROM `creature_addon` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+77;
INSERT INTO `creature_addon` (`guid`, `Path_Id`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(@CGUID+0, 0, 0, 0, 0, 0, 1, 0, 173, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Rogers
(@CGUID+4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132223'), -- Generic Controller Bunny (CSA) - 132223 - FX_AllianceGunship_Idle_Main_Loop
(@CGUID+7, 0, 0, 0, 0, 0, 1, 1, 483, 0, 0, 0, 0, ''), -- Captain Day
(@CGUID+9, 0, 0, 0, 0, 0, 1, 0, 69, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+10, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Corporal Rasmussen
(@CGUID+11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+12, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '131904'), -- Amber Kearnen - 131904 - Kearnen's Sniper Rifle
(@CGUID+13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+14, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+16, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+19, 0, 0, 0, 0, 0, 1, 0, 483, 0, 0, 0, 0, ''), -- Nodd Codejack
(@CGUID+21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+23, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 5, ''), -- Totally Generic Bunny (All Phase) - !!! already present in database !!!
(@CGUID+31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+34, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Sky Mage Harlan
(@CGUID+35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+38, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, '130860'), -- General Purpose Bunny ZTO - 130860 - Skyfire Defense Patrols
(@CGUID+39, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+41, 0, 0, 0, 0, 0, 1, 0, 69, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+46, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+47, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+51, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Hicks
(@CGUID+52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+54, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+55, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Ellis
(@CGUID+56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+57, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+66, 0, 0, 0, 3, 0, 1, 0, 416, 0, 0, 0, 0, ''), -- Skyfire Gyrocopter
(@CGUID+67, 0, 0, 0, 0, 0, 1, 1, 483, 0, 0, 0, 0, ''), -- Captain Kerwin
(@CGUID+68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Marine
(@CGUID+69, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Clifton
(@CGUID+70, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Sky Captain Callanan
(@CGUID+71, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 5, ''), -- Totally Generic Bunny (All Phase) - !!! already present in database !!!
(@CGUID+72, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+76, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+77, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'); -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop


-- Sky Captain Callanan entry 64859
SET @PATH := (@CGUID+70) * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -7880.561, 1309.455, 338.8661, NULL, 248),
(@PATH, 1, -7880.628, 1356.922, 338.8488, NULL, 58),
(@PATH, 2, -7880.561, 1309.455, 338.8661, NULL, 42),
(@PATH, 3, -7880.628, 1356.922, 338.8488, NULL, 59),
(@PATH, 4, -7880.561, 1309.455, 338.8661, NULL, 33),
(@PATH, 5, -7880.628, 1356.922, 338.8488, NULL, 26),
(@PATH, 6, -7880.561, 1309.455, 338.8661, NULL, 30),
(@PATH, 7, -7880.628, 1356.922, 338.8488, NULL, 23),
(@PATH, 8, -7880.561, 1309.455, 338.8661, NULL, 181),
(@PATH, 9, -7880.628, 1356.922, 338.8488, NULL, 48),
(@PATH, 10, -7880.561, 1309.455, 338.8661, NULL, 42),
(@PATH, 11, -7880.628, 1356.922, 338.8488, NULL, 4),
(@PATH, 12, -7880.561, 1309.455, 338.8661, NULL, 0);

UPDATE `creature` SET `position_x`= -7880.561, `position_y`= 1309.455, `position_z`= 338.8661, `orientation`= 0, `wander_distance`= 0, `MovementType`= 2 WHERE `guid`= @CGUID+70;
UPDATE `creature_addon` SET `Path_Id`=@PATH WHERE `guid`= @CGUID+70;

-- Skyfire Engineer entry 66480
SET @PATH := (@CGUID+2) * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -7898.217, 1334.785, 338.7588, NULL, 0),
(@PATH, 1, -7898.092, 1331.565, 338.7538, NULL, 0),
(@PATH, 2, -7898.2, 1311.845, 338.8352, NULL, 0),
(@PATH, 3, -7897.707, 1307.313, 338.8441, NULL, 0),
(@PATH, 4, -7896.303, 1306.513, 339.1078, NULL, 0),
(@PATH, 5, -7898.241, 1311.794, 338.8353, NULL, 0),
(@PATH, 6, -7898.234, 1331.564, 338.7529, NULL, 0),
(@PATH, 7, -7897.993, 1334.594, 338.7627, NULL, 0),
(@PATH, 8, -7897.354, 1347.019, 338.8489, NULL, 0),
(@PATH, 9, -7897.094, 1350.396, 338.8614, NULL, 0),
(@PATH, 10, -7896.434, 1354.962, 338.8368, NULL, 0),
(@PATH, 11, -7895.198, 1358.918, 339.2335, NULL, 0),
(@PATH, 12, -7890.538, 1365.399, 341.8928, NULL, 0),
(@PATH, 13, -7888.128, 1368.852, 341.7878, NULL, 0),
(@PATH, 14, -7886.337, 1370.811, 341.7793, NULL, 0),
(@PATH, 15, -7887.297, 1373.201, 341.8546, NULL, 0),
(@PATH, 16, -7891.354, 1365.448, 341.8876, NULL, 0),
(@PATH, 17, -7894.981, 1359.024, 339.2612, NULL, 0),
(@PATH, 18, -7896.505, 1354.563, 338.8389, NULL, 0),
(@PATH, 19, -7897.293, 1350.396, 338.8615, NULL, 0),
(@PATH, 20, -7897.603, 1347.063, 338.8851, NULL, 0),
(@PATH, 21, -7898.217, 1334.785, 338.7588, NULL, 0),
(@PATH, 22, -7898.092, 1331.565, 338.7538, NULL, 0),
(@PATH, 23, -7898.2, 1311.845, 338.8352, NULL, 0),
(@PATH, 24, -7898.128, 1309.794, 338.8607, NULL, 0),
(@PATH, 25, -7897.707, 1307.313, 338.8441, NULL, 0),
(@PATH, 26, -7896.303, 1306.513, 339.1078, NULL, 0),
(@PATH, 27, -7898.241, 1311.794, 338.8353, NULL, 0),
(@PATH, 28, -7898.234, 1331.564, 338.7529, NULL, 0),
(@PATH, 29, -7897.993, 1334.594, 338.7627, NULL, 0),
(@PATH, 30, -7897.354, 1347.019, 338.8489, NULL, 0),
(@PATH, 31, -7897.094, 1350.396, 338.8614, NULL, 0),
(@PATH, 32, -7896.434, 1354.962, 338.8368, NULL, 0),
(@PATH, 33, -7895.198, 1358.918, 339.2335, NULL, 0),
(@PATH, 34, -7890.538, 1365.399, 341.8928, NULL, 0),
(@PATH, 35, -7888.128, 1368.852, 341.7878, NULL, 0),
(@PATH, 36, -7886.337, 1370.811, 341.7793, NULL, 0),
(@PATH, 37, -7887.297, 1373.201, 341.8546, NULL, 0),
(@PATH, 38, -7891.354, 1365.448, 341.8876, NULL, 0),
(@PATH, 39, -7894.981, 1359.024, 339.2612, NULL, 0),
(@PATH, 40, -7896.505, 1354.563, 338.8389, NULL, 0),
(@PATH, 41, -7897.293, 1350.396, 338.8615, NULL, 0),
(@PATH, 42, -7897.603, 1347.063, 338.8851, NULL, 0),
(@PATH, 43, -7898.217, 1334.785, 338.7588, NULL, 0),
(@PATH, 44, -7898.092, 1331.565, 338.7538, NULL, 0),
(@PATH, 45, -7898.2, 1311.845, 338.8352, NULL, 0),
(@PATH, 46, -7898.128, 1309.794, 338.8607, NULL, 0),
(@PATH, 47, -7897.707, 1307.313, 338.8441, NULL, 0),
(@PATH, 48, -7896.303, 1306.513, 339.1078, NULL, 0),
(@PATH, 49, -7898.241, 1311.794, 338.8353, NULL, 0),
(@PATH, 50, -7898.234, 1331.564, 338.7529, NULL, 0),
(@PATH, 51, -7897.993, 1334.594, 338.7627, NULL, 0),
(@PATH, 52, -7897.354, 1347.019, 338.8489, NULL, 0),
(@PATH, 53, -7897.094, 1350.396, 338.8614, NULL, 0),
(@PATH, 54, -7896.434, 1354.962, 338.8368, NULL, 0),
(@PATH, 55, -7895.198, 1358.918, 339.2335, NULL, 0),
(@PATH, 56, -7890.538, 1365.399, 341.8928, NULL, 0),
(@PATH, 57, -7888.128, 1368.852, 341.7878, NULL, 0),
(@PATH, 58, -7886.337, 1370.811, 341.7793, NULL, 0),
(@PATH, 59, -7887.297, 1373.201, 341.8546, NULL, 0),
(@PATH, 60, -7891.354, 1365.448, 341.8876, NULL, 0),
(@PATH, 61, -7894.981, 1359.024, 339.2612, NULL, 0),
(@PATH, 62, -7896.505, 1354.563, 338.8389, NULL, 0),
(@PATH, 63, -7897.293, 1350.396, 338.8615, NULL, 0),
(@PATH, 64, -7897.603, 1347.063, 338.8851, NULL, 0),
(@PATH, 65, -7898.217, 1334.785, 338.7588, NULL, 0),
(@PATH, 66, -7898.092, 1331.565, 338.7538, NULL, 0),
(@PATH, 67, -7898.2, 1311.845, 338.8352, NULL, 0);

UPDATE `creature` SET `position_x`= -7898.217, `position_y`= 1334.785, `position_z`= 338.7588, `orientation`= 0, `wander_distance`= 0, `MovementType`= 2 WHERE `guid`= (@CGUID+2);
-- UPDATE `creature_addon` SET `Path_Id`=@PATH WHERE `guid`= (@CGUID+2);
DELETE FROM `creature_addon` WHERE `guid`= (@CGUID+2);
INSERT INTO `creature_addon` (`guid`, `path_id`) VALUES
(@CGUID+2, @PATH);

-- 0 2 6 9 41 
-- 2
SET @PATH := (@CGUID+73) * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -7864.146, 1347.302, 338.8495, NULL, 0),
(@PATH, 1, -7863.613, 1342.024, 338.7924, NULL, 0),
(@PATH, 2, -7863.128, 1334.852, 338.816, NULL, 0),
(@PATH, 3, -7862.993, 1331.79, 338.757, NULL, 0),
(@PATH, 4, -7862.799, 1317.009, 338.8095, NULL, 0),
(@PATH, 5, -7862.695, 1312.104, 338.8422, NULL, 0),
(@PATH, 6, -7862.851, 1310.105, 338.8466, NULL, 0),
(@PATH, 7, -7864.653, 1307.969, 339.0388, NULL, 0),
(@PATH, 8, -7862.738, 1312.103, 338.8422, NULL, 0),
(@PATH, 9, -7863.125, 1331.79, 338.7575, NULL, 0),
(@PATH, 10, -7863.12, 1334.849, 338.8204, NULL, 0),
(@PATH, 11, -7863.96, 1347.458, 338.9743, NULL, 0),
(@PATH, 12, -7864.802, 1350.595, 338.919, NULL, 0),
(@PATH, 13, -7868.318, 1359.075, 339.2714, NULL, 0),
(@PATH, 14, -7871.818, 1366.012, 341.8994, NULL, 0),
(@PATH, 15, -7876.188, 1374.488, 341.8718, NULL, 0),
(@PATH, 16, -7875.018, 1378.479, 341.8963, NULL, 0),
(@PATH, 17, -7868.268, 1358.91, 339.2043, NULL, 0),
(@PATH, 18, -7865.252, 1351.901, 338.8484, NULL, 0),
(@PATH, 19, -7864.146, 1347.302, 338.8495, NULL, 0),
(@PATH, 20, -7863.613, 1342.024, 338.7924, NULL, 0),
(@PATH, 21, -7863.128, 1334.852, 338.816, NULL, 0),
(@PATH, 22, -7862.993, 1331.79, 338.757, NULL, 0),
(@PATH, 23, -7862.799, 1317.009, 338.8095, NULL, 0),
(@PATH, 24, -7862.695, 1312.104, 338.8422, NULL, 0),
(@PATH, 25, -7862.851, 1310.105, 338.8466, NULL, 0),
(@PATH, 26, -7864.653, 1307.969, 339.0388, NULL, 0),
(@PATH, 27, -7862.738, 1312.103, 338.8422, NULL, 0),
(@PATH, 28, -7863.125, 1331.79, 338.7575, NULL, 0),
(@PATH, 29, -7863.12, 1334.849, 338.8204, NULL, 0),
(@PATH, 30, -7863.96, 1347.458, 338.9743, NULL, 0),
(@PATH, 31, -7864.802, 1350.595, 338.919, NULL, 0),
(@PATH, 32, -7868.318, 1359.075, 339.2714, NULL, 0),
(@PATH, 33, -7871.818, 1366.012, 341.8994, NULL, 0),
(@PATH, 34, -7876.188, 1374.488, 341.8718, NULL, 0),
(@PATH, 35, -7875.018, 1378.479, 341.8963, NULL, 0),
(@PATH, 36, -7868.268, 1358.91, 339.2043, NULL, 0),
(@PATH, 37, -7865.252, 1351.901, 338.8484, NULL, 0),
(@PATH, 38, -7864.146, 1347.302, 338.8495, NULL, 0),
(@PATH, 39, -7863.613, 1342.024, 338.7924, NULL, 0),
(@PATH, 40, -7863.128, 1334.852, 338.816, NULL, 0),
(@PATH, 41, -7862.993, 1331.79, 338.757, NULL, 0),
(@PATH, 42, -7862.799, 1317.009, 338.8095, NULL, 0),
(@PATH, 43, -7862.695, 1312.104, 338.8422, NULL, 0),
(@PATH, 44, -7862.851, 1310.105, 338.8466, NULL, 0),
(@PATH, 45, -7864.653, 1307.969, 339.0388, NULL, 0),
(@PATH, 46, -7862.738, 1312.103, 338.8422, NULL, 0),
(@PATH, 47, -7863.125, 1331.79, 338.7575, NULL, 0),
(@PATH, 48, -7863.12, 1334.849, 338.8204, NULL, 0),
(@PATH, 49, -7863.96, 1347.458, 338.9743, NULL, 0),
(@PATH, 50, -7864.802, 1350.595, 338.919, NULL, 0),
(@PATH, 51, -7868.318, 1359.075, 339.2714, NULL, 0),
(@PATH, 52, -7871.818, 1366.012, 341.8994, NULL, 0),
(@PATH, 53, -7876.188, 1374.488, 341.8718, NULL, 0),
(@PATH, 54, -7875.018, 1378.479, 341.8963, NULL, 0),
(@PATH, 55, -7871.852, 1366.344, 341.7703, NULL, 0),
(@PATH, 56, -7868.268, 1358.91, 339.2043, NULL, 0),
(@PATH, 57, -7865.252, 1351.901, 338.8484, NULL, 0),
(@PATH, 58, -7864.146, 1347.302, 338.8495, NULL, 0),
(@PATH, 59, -7863.613, 1342.024, 338.7924, NULL, 0),
(@PATH, 60, -7863.128, 1334.852, 338.816, NULL, 0),
(@PATH, 61, -7862.993, 1331.79, 338.757, NULL, 0);

UPDATE `creature` SET `position_x`= -7864.146, `position_y`= 1347.302, `position_z`= 338.8495, `orientation`= 0, `wander_distance`= 0, `MovementType`= 2 WHERE `guid`= @CGUID+73;
-- UPDATE `creature_addon` SET `Path_Id`=@PATH WHERE `guid`= @CGUID+2;
DELETE FROM `creature_addon` WHERE `guid`= (@CGUID+73);
INSERT INTO `creature_addon` (`guid`, `path_id`) VALUES
(@CGUID+73, @PATH);

-- Skyfire Marine entry 66300 13 24
SET @PATH := (@CGUID+13) * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -7918.184, 1278.201, 343.419, NULL, 0),
(@PATH, 1, -7909.846, 1279.118, 343.4194, NULL, 0),
(@PATH, 2, -7907.689, 1281.033, 343.4207, NULL, 0),
(@PATH, 3, -7907.675, 1284.469, 343.4159, NULL, 0),
(@PATH, 4, -7907.734, 1287.304, 342.5475, NULL, 0),
(@PATH, 5, -7907.151, 1290.3, 341.6843, NULL, 0),
(@PATH, 6, -7906.318, 1292.972, 341.079, NULL, 0),
(@PATH, 7, -7904.79, 1295.391, 340.2517, NULL, 0),
(@PATH, 8, -7902.833, 1297.823, 339.0133, NULL, 0),
(@PATH, 9, -7898.484, 1299.481, 338.8192, NULL, 0),
(@PATH, 10, -7893.649, 1300.028, 338.8591, NULL, 0),
(@PATH, 11, -7881.143, 1300.394, 338.8939, NULL, 0),
(@PATH, 12, -7873.092, 1300.528, 338.929, NULL, 0),
(@PATH, 13, -7864.866, 1300.524, 338.907, NULL, 0),
(@PATH, 14, -7861.866, 1299.931, 338.8307, NULL, 0),
(@PATH, 15, -7861.866, 1299.931, 338.8307, NULL, 0),
(@PATH, 16, -7861.866, 1299.931, 338.8307, NULL, 0),
(@PATH, 17, -7861.866, 1299.931, 338.8307, NULL, 0),
(@PATH, 18, -7857.518, 1298.873, 338.9271, NULL, 0),
(@PATH, 19, -7855.948, 1296.469, 340.0026, NULL, 0),
(@PATH, 20, -7854.58, 1293.767, 340.9899, NULL, 0),
(@PATH, 21, -7853.491, 1291.102, 341.6073, NULL, 0),
(@PATH, 22, -7852.777, 1288.286, 342.3914, NULL, 0),
(@PATH, 23, -7852.734, 1285.504, 343.2705, NULL, 0),
(@PATH, 24, -7852.34, 1281.38, 343.4205, NULL, 0),
(@PATH, 25, -7849.528, 1279.859, 343.4193, NULL, 0),
(@PATH, 26, -7837.253, 1276.924, 343.4189, NULL, 0),
(@PATH, 27, -7846.018, 1279.769, 343.4193, NULL, 0),
(@PATH, 28, -7852.905, 1282.254, 343.421, NULL, 0),
(@PATH, 29, -7852.759, 1284.957, 343.4159, NULL, 0),
(@PATH, 30, -7852.844, 1287.606, 342.6082, NULL, 0),
(@PATH, 31, -7853.229, 1290.332, 341.7832, NULL, 0),
(@PATH, 32, -7853.87, 1292.938, 341.2007, NULL, 0),
(@PATH, 33, -7855.224, 1295.969, 340.3858, NULL, 0),
(@PATH, 34, -7858.026, 1298.47, 338.877, NULL, 0),
(@PATH, 35, -7861.931, 1299.847, 338.834, NULL, 0),
(@PATH, 36, -7872.624, 1300.335, 338.8302, NULL, 0),
(@PATH, 37, -7879.412, 1300.306, 338.7977, NULL, 0),
(@PATH, 38, -7889.403, 1300.116, 338.837, NULL, 0),
(@PATH, 39, -7898.603, 1299.717, 338.8099, NULL, 0),
(@PATH, 40, -7902.866, 1298.318, 338.8956, NULL, 0),
(@PATH, 41, -7904.924, 1296.033, 340.0885, NULL, 0),
(@PATH, 42, -7906.042, 1293.042, 341.0515, NULL, 0),
(@PATH, 43, -7906.772, 1290.102, 341.7087, NULL, 0),
(@PATH, 44, -7907.167, 1287.314, 342.5593, NULL, 0),
(@PATH, 45, -7908.307, 1281.337, 343.4207, NULL, 0),
(@PATH, 46, -7913.603, 1278.858, 343.4189, NULL, 0),
(@PATH, 47, -7924.332, 1276.031, 343.4189, NULL, 0),
(@PATH, 48, -7918.184, 1278.201, 343.419, NULL, 0),
(@PATH, 49, -7909.846, 1279.118, 343.4194, NULL, 0),
(@PATH, 50, -7907.689, 1281.033, 343.4207, NULL, 0),
(@PATH, 51, -7907.675, 1284.469, 343.4159, NULL, 0),
(@PATH, 52, -7907.734, 1287.304, 342.5475, NULL, 0),
(@PATH, 53, -7907.151, 1290.3, 341.6843, NULL, 0),
(@PATH, 54, -7906.318, 1292.972, 341.079, NULL, 0),
(@PATH, 55, -7904.79, 1295.391, 340.2517, NULL, 0),
(@PATH, 56, -7902.833, 1297.823, 339.0133, NULL, 0),
(@PATH, 57, -7898.484, 1299.481, 338.8192, NULL, 0),
(@PATH, 58, -7893.649, 1300.028, 338.8591, NULL, 0),
(@PATH, 59, -7881.143, 1300.394, 338.8939, NULL, 0),
(@PATH, 60, -7873.092, 1300.528, 338.929, NULL, 0),
(@PATH, 61, -7864.866, 1300.524, 338.907, NULL, 0),
(@PATH, 62, -7861.866, 1299.931, 338.8307, NULL, 0),
(@PATH, 63, -7857.518, 1298.873, 338.9271, NULL, 0),
(@PATH, 64, -7855.948, 1296.469, 340.0026, NULL, 0),
(@PATH, 65, -7854.58, 1293.767, 340.9899, NULL, 0),
(@PATH, 66, -7853.491, 1291.102, 341.6073, NULL, 0),
(@PATH, 67, -7852.777, 1288.286, 342.3914, NULL, 0),
(@PATH, 68, -7852.734, 1285.504, 343.2705, NULL, 0),
(@PATH, 69, -7852.34, 1281.38, 343.4205, NULL, 0),
(@PATH, 70, -7849.528, 1279.859, 343.4193, NULL, 0),
(@PATH, 71, -7837.253, 1276.924, 343.4189, NULL, 0),
(@PATH, 72, -7846.018, 1279.769, 343.4193, NULL, 0),
(@PATH, 73, -7852.905, 1282.254, 343.421, NULL, 0),
(@PATH, 74, -7852.759, 1284.957, 343.4159, NULL, 0),
(@PATH, 75, -7852.844, 1287.606, 342.6082, NULL, 0),
(@PATH, 76, -7853.229, 1290.332, 341.7832, NULL, 0),
(@PATH, 77, -7853.87, 1292.938, 341.2007, NULL, 0),
(@PATH, 78, -7855.224, 1295.969, 340.3858, NULL, 0),
(@PATH, 79, -7858.026, 1298.47, 338.877, NULL, 0),
(@PATH, 80, -7861.931, 1299.847, 338.834, NULL, 0),
(@PATH, 81, -7872.624, 1300.335, 338.8302, NULL, 0),
(@PATH, 82, -7879.412, 1300.306, 338.7977, NULL, 0),
(@PATH, 83, -7889.403, 1300.116, 338.837, NULL, 0),
(@PATH, 84, -7898.603, 1299.717, 338.8099, NULL, 0),
(@PATH, 85, -7902.866, 1298.318, 338.8956, NULL, 0),
(@PATH, 86, -7904.924, 1296.033, 340.0885, NULL, 0),
(@PATH, 87, -7906.042, 1293.042, 341.0515, NULL, 0),
(@PATH, 88, -7906.772, 1290.102, 341.7087, NULL, 0),
(@PATH, 89, -7907.167, 1287.314, 342.5593, NULL, 0),
(@PATH, 90, -7908.307, 1281.337, 343.4207, NULL, 0),
(@PATH, 91, -7913.603, 1278.858, 343.4189, NULL, 0),
(@PATH, 92, -7924.332, 1276.031, 343.4189, NULL, 0),
(@PATH, 93, -7918.184, 1278.201, 343.419, NULL, 0),
(@PATH, 94, -7909.846, 1279.118, 343.4194, NULL, 0);

UPDATE `creature` SET `position_x`= -7918.184, `position_y`= 1278.201, `position_z`= 343.419, `orientation`= 0, `wander_distance`= 0, `MovementType`= 2 WHERE `guid`= @CGUID+13;
UPDATE `creature_addon` SET `Path_Id`=@PATH WHERE `guid`= @CGUID+13;


SET @PATH := (@CGUID+24) * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -7913.793, 1272.946, 343.631, NULL, 0),
(@PATH, 1, -7896.021, 1272.911, 357.8981, NULL, 0),
(@PATH, 2, -7877.344, 1272.96, 358.4773, NULL, 0),
(@PATH, 3, -7864.49, 1273.271, 357.9572, NULL, 0),
(@PATH, 4, -7846.616, 1273.462, 343.7882, NULL, 0),
(@PATH, 5, -7838.893, 1273.733, 343.4214, NULL, 0),
(@PATH, 6, -7846.41, 1273.736, 343.6446, NULL, 0),
(@PATH, 7, -7863.997, 1272.894, 357.7568, NULL, 0),
(@PATH, 8, -7878.991, 1273.036, 358.4794, NULL, 0),
(@PATH, 9, -7894.147, 1273.177, 358.0936, NULL, 0),
(@PATH, 10, -7895.984, 1272.773, 357.9295, NULL, 0),
(@PATH, 11, -7913.762, 1272.842, 343.6524, NULL, 0),
(@PATH, 12, -7921.799, 1273.422, 343.421, NULL, 0),
(@PATH, 13, -7913.793, 1272.946, 343.631, NULL, 0),
(@PATH, 14, -7896.021, 1272.911, 357.8981, NULL, 0),
(@PATH, 15, -7877.344, 1272.96, 358.4773, NULL, 0),
(@PATH, 16, -7864.49, 1273.271, 357.9572, NULL, 0),
(@PATH, 17, -7846.616, 1273.462, 343.7882, NULL, 0),
(@PATH, 18, -7838.893, 1273.733, 343.4214, NULL, 0),
(@PATH, 19, -7846.41, 1273.736, 343.6446, NULL, 0),
(@PATH, 20, -7863.997, 1272.894, 357.7568, NULL, 0),
(@PATH, 21, -7878.991, 1273.036, 358.4794, NULL, 0),
(@PATH, 22, -7894.147, 1273.177, 358.0936, NULL, 0),
(@PATH, 23, -7895.984, 1272.773, 357.9295, NULL, 0),
(@PATH, 24, -7913.762, 1272.842, 343.6524, NULL, 0),
(@PATH, 25, -7921.799, 1273.422, 343.421, NULL, 0),
(@PATH, 26, -7913.793, 1272.946, 343.631, NULL, 0),
(@PATH, 27, -7896.021, 1272.911, 357.8981, NULL, 0),
(@PATH, 28, -7877.344, 1272.96, 358.4773, NULL, 0),
(@PATH, 29, -7864.49, 1273.271, 357.9572, NULL, 0),
(@PATH, 30, -7846.616, 1273.462, 343.7882, NULL, 0),
(@PATH, 31, -7838.893, 1273.733, 343.4214, NULL, 0),
(@PATH, 32, -7846.41, 1273.736, 343.6446, NULL, 0),
(@PATH, 33, -7863.997, 1272.894, 357.7568, NULL, 0),
(@PATH, 34, -7878.991, 1273.036, 358.4794, NULL, 0);

UPDATE `creature` SET `position_x`= -7913.793, `position_y`= 1272.946, `position_z`= 343.631, `orientation`= 0, `wander_distance`= 0, `MovementType`= 2 WHERE `guid`= @CGUID+24;
UPDATE `creature_addon` SET `Path_Id`=@PATH WHERE `guid`= @CGUID+24;

-- Captain Day entry 64861
-- SAI missing
-- @CGUID+7

-- gameobjects on skyfire
SET @OGUID=4000070;
DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+6;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `PhaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID+0, 215648, 0, 1519, 6555, 1, 1686, 0, -7894.48974609375, 1354.6822509765625, 339.892852783203125, 0, 0, 0, 0, 1, 120, 255, 1, 55261), -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@OGUID+1, 215648, 0, 1519, 6555, 1, 1686, 0, -7892.52783203125, 1354.90625, 338.81634521484375, 0, 0, 0, 0, 1, 120, 255, 1, 55261), -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@OGUID+2, 215648, 0, 1519, 6555, 1, 1686, 0, -7893.98095703125, 1355.5555419921875, 338.81695556640625, 0, 0, 0, 0, 1, 120, 255, 1, 55261), -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@OGUID+3, 216056, 0, 1519, 6555, 1, 1686, 0, -7879.80029296875, 1279.515625, 358.560516357421875, 4.728822231292724609, 0, 0, -0.70127296447753906, 0.712892889976501464, 120, 255, 1, 55261), -- Collision PC Size (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1 - !!! might be temporary spawn !!!
(@OGUID+4, 215648, 0, 1519, 6555, 1, 1686, 0, -7892.501953125, 1353.515625, 338.827392578125, 0, 0, 0, 0, 1, 120, 255, 1, 55261), -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@OGUID+5, 215648, 0, 1519, 6555, 1, 1686, 0, -7895.234375, 1354.65283203125, 338.831878662109375, 0, 0, 0, 0, 1, 120, 255, 1, 55261), -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
(@OGUID+6, 215648, 0, 1519, 6555, 1, 1686, 0, -7893.88525390625, 1354.171875, 338.818756103515625, 0, 0, 0, 0, 1, 120, 255, 1, 55261); -- Medical Supply Crate (Area: Northern Elwynn Mountains - Difficulty: 0) CreateObject1
DELETE FROM `gameobject_addon` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+6;

UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=55830; -- Nimm Codejack
UPDATE `creature_template` SET `unit_flags`=0, `unit_flags2`=2048 WHERE `entry`=64859; -- Sky Captain Callanan
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64864; -- Airman Clifton
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64860; -- Captain Kerwin
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=55823; -- Mishka
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64862; -- Airman Ellis
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64858; -- Airman Hicks
UPDATE `creature_template` SET `unit_flags`=33554432 WHERE `entry`=55370; -- General Purpose Bunny ZTO
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64863; -- Sky Mage Harlan
UPDATE `creature_template` SET `unit_flags2`=2048 WHERE `entry`=32520; -- Totally Generic Bunny (All Phase)
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=55819; -- Sully "The Pickle" McLeary
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=55832; -- Nodd Codejack
-- UPDATE `creature_template` SET `unit_flags`=0, `unit_flags3`=16777217 WHERE `entry`=66473; -- Skyfire Gyrocopter
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=55825; -- Amber Kearnen
UPDATE `creature_template` SET `unit_flags`=0, `unit_flags2`=2048 WHERE `entry`=66300; -- Skyfire Marine
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64866; -- Corporal Rasmussen
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64861; -- Captain Day
UPDATE `creature_template` SET `npcflag`=3, `BaseAttackTime`=2000, `unit_flags`=256, `unit_flags2`=2048 WHERE `entry`=66292; -- Sky Admiral Rogers
-- UPDATE `creature_template` SET `unit_flags3`=16777216 WHERE `entry`=40789; -- Generic Controller Bunny (CSA)
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64865; -- Airman Rogers
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=66460; -- Skyfire Gyrocopter Pilot
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry`=66480; -- Skyfire Engineer

-- Remove incorrect auras apply on all 66300 entry
UPDATE `creature_template_addon` SET `auras`='' WHERE `entry`=66300;

-- Creature text for Nodd Codejack <SI:7> 55832
DELETE FROM `creature_text` WHERE `CreatureID`=55832;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(55832, 0, 0, '13 gyromantic destabilizers? Check. 10 gnumatic flangegrips? Check. Now where did I put my favorite wrench?', 12, 0, 100, 0, 0, 0, 67697, 0, 'Nodd Codejack <SI:7>'),
(55832, 0, 1, 'Hmm. Only 2 tons of explosives for this mission? Seems a bit light to me. This seems like the kind of mission we are going to need way more explosives for. Like... goblin pyromaniac explosives.', 12, 0, 100, 0, 0, 0, 67698, 0, 'Nodd Codejack <SI:7>'),
(55832, 0, 2, 'Only thirty gyrocopters? We are going to need way more than that before this is over.', 12, 0, 100, 0, 0, 0, 67699, 0, 'Nodd Codejack <SI:7>');

-- SAI 
-- Sully "The Pickle" McLeary 55819
-- Mishka 55823
-- Nimm Codejack 55830
-- Nodd Codejack <SI:7> 55832 , text timeline maybe wrong
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (55819,55823,55830,55832);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(55819, 0, 0, 0, 1, 0, 100, 0, 4000, 8000, 4000, 8000, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '\"The Pickle\" McLeary - Out of Combat - Play Emote 1'),
(55823, 0, 0, 0, 1, 0, 100, 0, 4000, 8000, 4000, 8000, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Mishka - Out of Combat - Play Emote 1'),
(55830, 0, 0, 0, 1, 0, 100, 0, 4000, 8000, 4000, 8000, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Nimm Codejack - Out of Combat - Play Emote 1'),
(55832, 0, 0, 0, 1, 0, 100, 0, 5000, 10000, 50000, 70000, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Nodd Codejack <SI:7> - OOC - talking');

UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN (55819,55823,55830,55832);