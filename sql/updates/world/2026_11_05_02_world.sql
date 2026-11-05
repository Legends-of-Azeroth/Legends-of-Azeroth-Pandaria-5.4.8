-- Ashenvale, Splintertree Post, quest 13798 "Rain of Destruction": the siege attackers (Ashenvale Assailant 33945, Ashenvale Bowman 33914,
-- Raging Ancient 33912; 39 spawns) are in phase mask 1 while the defenders (Splintertree Guard, Demolisher, Draaka, Durak and 6 bowmen) are
-- already in phase mask 2. Players without the quest see the attackers standing around without defenders; players wearing the quest disguise
-- (Imp Disguise 63704, which swaps the phase) do not see the attackers at all, so the quest targets are missing.
-- Move the attackers to phase mask 2 like the defenders.
-- Source: Wowhead quest 13798 (the siege of Splintertree Post is seen during the quest).
UPDATE `creature` SET `phaseMask` = 2 WHERE `id` IN (33945,33914,33912) AND `phaseMask` = 1 AND `guid` IN (311142,311143,311144,311145,311189,311196,311197,311198,311199,311200,311201,311202,311179,311180,311182,311183,311184,311186,311187,311188,311190,311191,311192,311193,311194,311227,311228,311230,311231,311232,311233,311236,311237,311239,311242,311229,311234,311235,311240);
