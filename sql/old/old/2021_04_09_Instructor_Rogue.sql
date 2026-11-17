-- La doble especialización de talentos costaba mil de oro porque tenía un script asociado el cual controlaba la misión 6681
-- la misión fue desactivada por blizzard y el resto de opciones del script ya están asociados al npc mediante Base de datos
-- fuente https://www.wowhead.com/quest=6681/the-manor-ravenholdt
UPDATE `creature_template` SET `ScriptName` = '' WHERE `ScriptName` = 'npc_rogue_trainer';
