-- pandaren warrior
-- start with action bar: attack, heroic strike, quaking palm, small rice cake
delete from playercreateinfo_action where race = 24 and class = 1;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 1, 0, 88163, 0),
    (24, 1, 1, 78, 0),
    (24, 1, 9, 107079, 0),
    (24, 1, 11, 77273, 128),
    (24, 1, 72, 88163, 0);

-- pandaren hunter
-- start with action bar: arcane shot, small sugarcane stalk, quaking palm, call pet, revive pet
delete from playercreateinfo_action where race = 24 and class = 3;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 3, 0, 3044, 0),
    (24, 3, 7, 77272, 128),
    (24, 3, 9, 107079, 0),
    (24, 3, 10, 9, 48),
    (24, 3, 11, 982, 0);

-- pandaren rogue
-- start with action bar: sinister strike, quaking palm, small sugarcane stalk
delete from playercreateinfo_action where race = 24 and class = 4;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 4, 0, 1752, 0),
    (24, 4, 9, 107079, 0),
    (24, 4, 11, 77272, 128);

-- pandaren priest
-- start with action bar: smite, quaking palm, small bamboo shoot
delete from playercreateinfo_action where race = 24 and class = 5;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 5, 0, 585, 0),
    (24, 5, 9, 107079, 0),
    (24, 5, 11, 77264, 128);

-- pandaren shaman
-- start with action bar: lightning bolt, quaking palm, small sugarcane stalk
delete from playercreateinfo_action where race = 24 and class = 7;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 7, 0, 403, 0),
    (24, 7, 9, 107079, 0),
    (24, 7, 11, 77272, 128),
    (24, 7, 72, 403, 0);

-- pandaren mage
-- start with action bar: frostfire bolt, quaking palm, small bamboo shoot
delete from playercreateinfo_action where race = 24 and class = 8;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 8, 0, 44614, 0),
    (24, 8, 9, 107079, 0),
    (24, 8, 11, 77264, 128);

-- pandaren monk
-- start with action bar: jab, quaking palm, small sugarcane stalk
delete from playercreateinfo_action where race = 24 and class = 10;
insert into playercreateinfo_action (race, class, button, action, type) values
    (24, 10, 0, 100780, 0),
    (24, 10, 9, 107079, 0),
    (24, 10, 11, 77272, 128),
    (24, 10, 72, 100780, 0),
    (24, 10, 81, 107079, 0),
    (24, 10, 83, 77272, 128);

-- pandaren monk
-- cast stance of the fierce tiger
delete from playercreateinfo_cast_spell where raceMask = 8388608 and classMask = 512;
insert into playercreateinfo_cast_spell (raceMask, classMask, spell, note) values
    (8388608, 512, 107915, 'Pandaren - Monk - Starting quest'),
    (8388608, 512, 108060, 'Pandaren - Monk - Remove weapon'),
    (8388608, 512, 103985, 'Pandaren - Monk - Stance of the Fierce Tiger');
