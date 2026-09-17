-- la carga de puntos no sucedia porque el valor inicial de la columna era null, esto se cambio por un CERO
UPDATE `account` SET dp = 0; 
ALTER TABLE `account` CHANGE `dp` `dp` INT(10) DEFAULT 0 NOT NULL;
