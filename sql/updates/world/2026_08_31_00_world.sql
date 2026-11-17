-- 28528 Vengeful Ghoul (Eye of Acherus quest 12641, summoned via 51904 -> 54522).
-- Faction 2084 (Scourge) in this client's FactionTemplate.dbc is neutral to Blood
-- Knights (faction 14) and is only one-way hostile to Scarlet Crusade (FT67), so the
-- ghouls never engage the Scarlet aggressors (and are not engaged back). Switch to
-- faction 1 (canonical hostile creature): mutually hostile to Scarlet Crusade (67)
-- and Blood Knights (14), while not targetable by the player's own Scourge allies
-- (faction 2084 keeps ourMask 0x0). The player is not physically present at the
-- summon point during the Eye-of-Acherus fly-over, so faction 1's player-hostility is
-- a non-issue here.
UPDATE `creature_template` SET `faction` = 1 WHERE `entry` = 28528;
