-- Repoint the archived auth updates to the non-nested archive location.
-- They were nested under the RELEASED scan root (sql/updates/master/auth/old),
-- which the recursive update scanner visits twice -> fatal "Duplicate filename".
UPDATE `updates_include` SET `path` = '$/sql/archive/auth' WHERE `path` = '$/sql/updates/master/auth/old';
