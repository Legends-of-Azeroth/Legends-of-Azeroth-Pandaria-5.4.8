-- Corpse guids are map specific, so corpses on two maps can share a corpseGuid. A player has at most one saved corpse
-- (Corpse::SaveToDB and DeleteFromDB go by owner), so key the table by the owner instead.
ALTER TABLE `corpse` DROP PRIMARY KEY, ADD PRIMARY KEY (`guid`);
