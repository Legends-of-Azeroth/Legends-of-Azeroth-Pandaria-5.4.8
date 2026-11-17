-- Summon Darnell (91938) is no longer saved: drop saved copies, which would keep spell_area from summoning Darnell again on login
DELETE FROM `character_aura` WHERE `spell` = 91938;

-- Summon Lilian Voss (Guardian) (73306) is no longer saved: drop saved copies, which would keep spell_area from summoning Lilian again on login
DELETE FROM `character_aura` WHERE `spell` = 73306;
