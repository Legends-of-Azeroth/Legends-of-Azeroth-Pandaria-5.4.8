-- !!! only rename once, before anything else !!!
-- rename table world.quest_poi TO world.quest_poi_mop;
-- rename table world.quest_poi_points TO world.quest_poi_points_mop;


use world;

--
-- Table structure for table `quest_poi`
--

DROP TABLE IF EXISTS `quest_poi`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_poi` (
  `QuestID` int unsigned NOT NULL DEFAULT '0',
  `Idx1` int unsigned NOT NULL DEFAULT '0',
  `ObjectiveIndex` int NOT NULL DEFAULT '0',
  `QuestObjectiveId` int unsigned NOT NULL DEFAULT '0',
  `MapID` int unsigned NOT NULL DEFAULT '0',
  `WorldMapAreaId` int unsigned NOT NULL DEFAULT '0',
  `Floor` int unsigned NOT NULL DEFAULT '0',
  `Priority` int unsigned NOT NULL DEFAULT '0',
  `Flags` int unsigned NOT NULL DEFAULT '0',
  `VerifiedBuild` smallint DEFAULT '0',
  PRIMARY KEY (`QuestID`,`Idx1`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_poi_points`
--

DROP TABLE IF EXISTS `quest_poi_points`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_poi_points` (
  `QuestID` int NOT NULL DEFAULT '0',
  `BlobIndex` int NOT NULL DEFAULT '0',
  `Idx1` int NOT NULL DEFAULT '0',
  `Idx2` int NOT NULL DEFAULT '0',
  `X` int NOT NULL DEFAULT '0',
  `Y` int NOT NULL DEFAULT '0',
  `VerifiedBuild` smallint DEFAULT '0',
  PRIMARY KEY (`QuestID`,`BlobIndex`,`Idx1`,`Idx2`),
  KEY `questId_id` (`QuestID`,`BlobIndex`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;


-- copy all cata poi data
INSERT INTO world.quest_poi (QuestID, Idx1, ObjectiveIndex, MapID, WorldMapAreaId, Floor, Priority, Flags, VerifiedBuild)
SELECT QuestID, id, ObjectiveIndex, MapID, WorldMapAreaId, Floor, Priority, Flags, 0
FROM cata.quest_poi;

-- copy all cata poi points data
INSERT INTO world.quest_poi_points (QuestID, BlobIndex, Idx1, Idx2, X, Y, VerifiedBuild)
SELECT QuestID, BlobIndex, Idx1, Idx2, X, Y, 0
FROM cata.quest_poi_points;

-- drop poi and poi points data that we have from wotlk (outland and northrend)
DELETE world.quest_poi
FROM world.quest_poi
INNER JOIN wotlk.quest_poi ON wotlk.quest_poi.QuestID = world.quest_poi.QuestID
WHERE wotlk.quest_poi.MapID IN (530, 571);

-- drop orphan poi points
DELETE world.quest_poi_points
FROM world.quest_poi_points
WHERE NOT EXISTS (SELECT 1 FROM world.quest_poi WHERE world.quest_poi_points.QuestID = world.quest_poi.QuestID);

-- copy all wotlk poi data (outland and northrend)
INSERT INTO world.quest_poi (QuestID, Idx1, ObjectiveIndex, MapID, WorldMapAreaId, Floor, Priority, Flags, VerifiedBuild)
SELECT QuestID, id, ObjectiveIndex, MapID, WorldMapAreaId, Floor, Priority, Flags, 0
FROM wotlk.quest_poi
WHERE wotlk.quest_poi.MapID IN (530, 571);

-- copy all wotlk poi points data (outland and northrend)
INSERT INTO world.quest_poi_points (QuestID, BlobIndex, Idx1, Idx2, X, Y, VerifiedBuild)
SELECT wotlk.quest_poi_points.QuestID, Idx1, Idx1, Idx2, X, Y, 0
FROM wotlk.quest_poi_points
WHERE EXISTS (SELECT 1 FROM wotlk.quest_poi WHERE wotlk.quest_poi.QuestID = wotlk.quest_poi_points.QuestID AND wotlk.quest_poi.MapID IN (530, 571));

-- copy all pandaria poi data
INSERT INTO world.quest_poi (QuestID, Idx1, ObjectiveIndex, MapID, WorldMapAreaId, Floor, Priority, Flags, VerifiedBuild)
SELECT QuestID, id, objIndex, MapID, WorldMapAreaId, FloorId, unk3, unk4, 0
FROM world.quest_poi_mop WHERE world.quest_poi_mop.mapid = 870;

-- copy all pandaria poi points data
INSERT INTO world.quest_poi_points (QuestID, BlobIndex, Idx1, Idx2, X, Y, VerifiedBuild)
SELECT QuestID, id, id, idx, X, Y, 0
FROM world.quest_poi_points_mop
WHERE EXISTS (SELECT 1 FROM world.quest_poi_mop WHERE world.quest_poi_mop.QuestID = world.quest_poi_points_mop.QuestID AND world.quest_poi_mop.MapID = 870);

-- update all quest objective id data from TDB master
UPDATE world.quest_poi qp1
INNER JOIN tdb.quest_poi qp2 ON qp1.QuestId = qp2.QuestID AND qp1.ObjectiveIndex = qp2.ObjectiveIndex
SET qp1.QuestObjectiveId = qp2.QuestObjectiveID
WHERE qp1.QuestID > 0; -- to disable unsafe query warning


-- Quest 8330 - Solanian's Belongings
-- fix quest poi objective/index
set @ENTRY := 8330;
update quest_poi set QuestObjectiveId = 256353 where questId = @ENTRY and Idx1 = 0;
update quest_poi set QuestObjectiveId = 256354 where questId = @ENTRY and Idx1 = 1;
update quest_poi set QuestObjectiveId = 256355 where questId = @ENTRY and Idx1 = 2;
