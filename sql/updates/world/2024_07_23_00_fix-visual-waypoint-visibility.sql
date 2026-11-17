-- non-zero model id > 1 makes the creature visible to non-gms
-- VISUAL_WAYPOINT should only be visible to gms
update creature_template
set modelid1 = 10045,
    modelid2 = 0,
    modelid3 = 0,
    modelid4 = 0
where entry = 1;