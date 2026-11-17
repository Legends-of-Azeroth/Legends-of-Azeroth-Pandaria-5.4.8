-- ajustando coordenadas del jefe para que no se vaya al fondo de la plataforma
UPDATE `creature` SET `position_x` = '4025.139' , `position_y` = '1907.31' , `position_z` = '361.788' WHERE `guid` = '552324'; 
-- ajustando el estado del jefe para volverlo una criatura voladora
UPDATE `creature_template` SET `InhabitType` = '4' WHERE `entry` = '60410'; 
