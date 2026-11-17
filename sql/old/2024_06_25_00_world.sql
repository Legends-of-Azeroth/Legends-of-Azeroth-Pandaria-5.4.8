-- NPC 15295 - Well Watcher Solanian
-- fix position; uses data from cata db
update creature set position_x = 10376.4, position_y = -6406.69, position_z = 49.798, orientation = 3.58665 where id = 15295;

-- Creature 15645 - Wretched Thug
-- Creature 16162 - Wretched Hooligan
-- Creature 15650 - Crazed Dragonhawk
-- Creature 15651 - Springpaw Stalker
-- fix creatures with no movement
update creature set movement_type = 1, wander_distance = 5 where id in (15645, 16162, 15650, 15651);

-- Creature 15650 - Crazed Dragonhawk
-- fix random movement to have delays (because creature doesn't fly/inhabit air)
update creature_template set InhabitType = 3 where entry = 15650;

-- remove UNIT_FLAG_PVP_ATTACKABLE except where flag exists in wotlk and cata dbs
-- fix creature names staying red after they are killed; training dummy names showing red, etc
update creature_template set unit_flags = unit_flags & ~0x8
where
    unit_flags & 0x8
  -- from wotlk db
  and entry not in (12427, 12428, 12429, 14484, 14485, 23134, 23135, 23136, 25460, 27923, 28120, 28121, 28122, 28214, 28215, 28216, 28222, 28521, 28665, 28927, 28948, 29018, 29579, 29709, 29884, 30564, 30645, 31110, 31261, 31268, 31276, 33519)
  -- from cata db
  and entry not in (17551, 24916, 24958, 25460, 27923, 28120, 28121, 28122, 28214, 28215, 28216, 28222, 28511, 28521, 28665, 28817, 28927, 28948, 29018, 29579, 29709, 29884, 30161, 30564, 30645, 30835, 30836, 31110, 31268, 31276, 33248, 33519, 36790, 37198, 39128, 39622, 40180, 42492, 42500, 42515, 42617, 43161, 43484, 43497, 43499, 44199, 44214, 44795, 45322, 47649, 48115, 48165, 48633, 48634, 48636, 48637, 48638, 48779, 48782, 48823, 48827, 51098, 51125);
