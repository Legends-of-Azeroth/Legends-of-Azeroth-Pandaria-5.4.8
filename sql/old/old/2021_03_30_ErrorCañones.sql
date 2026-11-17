
-- gobId 28833 , zona dk	, VehicleID: 139
-- gobId 27894 , zona playa Bg	, VehicleID: 453
-- gobId 35317 , zona gilenas  	, VehicleID: 470
-- se cambio por el id 139 por el id 453, el anterior permitia mover el vehículo por el mapa
UPDATE `creature_template` SET `VehicleId` = '453' WHERE `entry` = '28833';
