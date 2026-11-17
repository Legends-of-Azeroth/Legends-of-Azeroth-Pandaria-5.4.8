-- Silverpine Forest: Horde Hauler (44731) and Horde Coffin Hauler (44764) drive their waypoint path without the ettin that pulls them.
-- The Subdued Forest Ettins (44737 / 46560) stand next to the carts as separate wandering spawns. Install them as vehicle accessory
-- on seat 1 (the front/yoke seat, attachment 15) and remove the 18 loose spawns.
-- Source: Vehicle/VehicleSeat DBC (seat 0 is the driver position on the cart, seat 1 the front position); Warcraft Wiki, Horde Hauler:
-- "pulled by a Subdued Forest Ettin".
DELETE FROM `vehicle_template_accessory` WHERE `entry` IN (44731, 44764);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(44731, 44737, 1, 1, 'Horde Hauler - Subdued Forest Ettin', 8, 0),
(44764, 46560, 1, 1, 'Horde Coffin Hauler - Subdued Forest Ettin', 8, 0);

DELETE FROM `creature` WHERE `guid` IN (187562,187573,187582,187591,187600,187630,187637,187644,188358,188036,188058,188087,188104,188124,188179,188195,188200,188222) AND `id` IN (44737, 46560);
