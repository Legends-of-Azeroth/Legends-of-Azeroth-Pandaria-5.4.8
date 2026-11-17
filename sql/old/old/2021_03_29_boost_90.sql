/*
    SPEC_MAGE_ARCANE                    = 62,
    SPEC_MAGE_FIRE                      = 63,
    SPEC_MAGE_FROST                     = 64,
    SPEC_PALADIN_HOLY                   = 65,
    SPEC_PALADIN_PROTECTION             = 66,
    SPEC_PALADIN_RETRIBUTION            = 70,
    SPEC_WARRIOR_ARMS                   = 71,
    SPEC_WARRIOR_FURY                   = 72,
    SPEC_WARRIOR_PROTECTION             = 73,
    SPEC_DRUID_BALANCE                  = 102,
    SPEC_DRUID_FERAL                    = 103,
    SPEC_DRUID_GUARDIAN                 = 104,
    SPEC_DRUID_RESTORATION              = 105,
    SPEC_DEATH_KNIGHT_BLOOD             = 250,
    SPEC_DEATH_KNIGHT_FROST             = 251,
    SPEC_DEATH_KNIGHT_UNHOLY            = 252,
    SPEC_HUNTER_BEAST_MASTERY           = 253,
    SPEC_HUNTER_MARKSMANSHIP            = 254,
    SPEC_HUNTER_SURVIVAL                = 255,
    SPEC_PRIEST_DISCIPLINE              = 256,
    SPEC_PRIEST_HOLY                    = 257,
    SPEC_PRIEST_SHADOW                  = 258,
    SPEC_ROGUE_ASSASSINATION            = 259,
    SPEC_ROGUE_COMBAT                   = 260,
    SPEC_ROGUE_SUBTLETY                 = 261,
    SPEC_SHAMAN_ELEMENTAL               = 262,
    SPEC_SHAMAN_ENHANCEMENT             = 263,
    SPEC_SHAMAN_RESTORATION             = 264,
    SPEC_WARLOCK_AFFLICTION             = 265,
    SPEC_WARLOCK_DEMONOLOGY             = 266,
    SPEC_WARLOCK_DESTRUCTION            = 267,
    SPEC_MONK_BREWMASTER                = 268,
    SPEC_MONK_WINDWALKER                = 269,
    SPEC_MONK_MISTWEAVER                = 270

*/

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '62';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '62';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '62' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '63';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '63';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '63' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '64';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '64';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '64' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83161 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83170 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83179 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83224 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83215 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83206 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83188 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83197 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83267 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '65';
UPDATE `battle_pay_boost_items` SET itemId = 83273 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '65';

UPDATE `battle_pay_boost_items` SET itemId = 83165 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83174 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83183 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83228 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83219 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83210 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83192 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83201 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83248 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83271 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '66';
UPDATE `battle_pay_boost_items` SET itemId = 83272 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '66';

UPDATE `battle_pay_boost_items` SET itemId = 83166 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83175 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83184 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83229 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83220 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83211 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83193 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83202 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '70';
UPDATE `battle_pay_boost_items` SET itemId = 83270 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '70';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '70' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83166 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83175 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83184 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83229 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83220 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83211 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83193 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83202 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '71';
UPDATE `battle_pay_boost_items` SET itemId = 83270 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '71';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '71' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83166 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83175 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83184 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83229 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83220 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83211 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83193 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83202 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83270 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '72';
UPDATE `battle_pay_boost_items` SET itemId = 83270 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '72';

UPDATE `battle_pay_boost_items` SET itemId = 83165 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83174 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83183 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83228 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83219 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83210 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83192 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83201 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83248 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83271 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '73';
UPDATE `battle_pay_boost_items` SET itemId = 83272 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '73';

UPDATE `battle_pay_boost_items` SET itemId = 83159 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83168 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83177 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83222 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83213 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83204 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83186 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83195 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '102';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '102';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '102' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '103';
UPDATE `battle_pay_boost_items` SET itemId = 83262 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '103';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '103' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '104';
UPDATE `battle_pay_boost_items` SET itemId = 83262 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '104';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '104' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83159 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83168 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83177 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83222 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83213 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83204 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83186 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83195 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '105';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '105';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '105' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83165 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83174 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83183 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83228 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83219 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83210 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83192 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83201 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83248 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '250';
UPDATE `battle_pay_boost_items` SET itemId = 83263 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '250';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '250' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83166 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83175 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83184 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83229 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83220 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83211 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83193 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83202 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83264 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '251';
UPDATE `battle_pay_boost_items` SET itemId = 83264 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '251';

UPDATE `battle_pay_boost_items` SET itemId = 83166 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83234 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83175 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83259 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83184 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83229 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83220 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83211 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83193 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83202 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83238 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83239 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83264 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '252';
UPDATE `battle_pay_boost_items` SET itemId = 83264 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '252';

UPDATE `battle_pay_boost_items` SET itemId = 83164 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83173 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83182 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83227 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83218 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83209 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83191 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83200 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '253';
UPDATE `battle_pay_boost_items` SET itemId = 83265 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '253';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '253' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83164 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83173 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83182 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83227 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83218 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83209 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83191 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83200 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '254';
UPDATE `battle_pay_boost_items` SET itemId = 83265 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '254';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '254' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83164 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83173 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83182 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83227 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83218 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83209 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83191 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83200 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '255';
UPDATE `battle_pay_boost_items` SET itemId = 83265 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '255';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '255' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '256';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '256';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '256' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '257';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '257';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '257' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '258';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '258';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '258' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83268 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '259';
UPDATE `battle_pay_boost_items` SET itemId = 83274 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '259';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83268 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '260';
UPDATE `battle_pay_boost_items` SET itemId = 83274 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '260';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83268 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '261';
UPDATE `battle_pay_boost_items` SET itemId = 83274 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '261';

UPDATE `battle_pay_boost_items` SET itemId = 83160 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83169 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83178 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83223 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83214 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83205 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83187 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83196 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83267 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '262';
UPDATE `battle_pay_boost_items` SET itemId = 83273 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '262';

UPDATE `battle_pay_boost_items` SET itemId = 83164 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83173 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83182 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83227 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83218 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83209 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83191 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83200 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83275 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '263';
UPDATE `battle_pay_boost_items` SET itemId = 83275 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '263';

UPDATE `battle_pay_boost_items` SET itemId = 83160 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83169 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83178 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83223 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83214 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83205 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83187 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83196 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83269 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '264';
UPDATE `battle_pay_boost_items` SET itemId = 83273 WHERE slot = 'EQUIPMENT_SLOT_OFFHAND' AND `specialization` = '264';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '265';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '265';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '265' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '266';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '266';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '266' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83158 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83167 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83176 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83221 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83216 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83203 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83185 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83194 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '267';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '267';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '267' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '268';
UPDATE `battle_pay_boost_items` SET itemId = 83262 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '268';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '268' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83163 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83232 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83172 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83257 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83181 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83226 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83217 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83208 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83190 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83199 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83237 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83249 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '269';
UPDATE `battle_pay_boost_items` SET itemId = 83262 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '269';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '269' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';

UPDATE `battle_pay_boost_items` SET itemId = 83159 WHERE slot = 'EQUIPMENT_SLOT_HEAD' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83231 WHERE slot = 'EQUIPMENT_SLOT_NECK' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83168 WHERE slot = 'EQUIPMENT_SLOT_SHOULDERS' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83256 WHERE slot = 'EQUIPMENT_SLOT_BACK' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83177 WHERE slot = 'EQUIPMENT_SLOT_CHEST' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83222 WHERE slot = 'EQUIPMENT_SLOT_WRISTS' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83213 WHERE slot = 'EQUIPMENT_SLOT_HANDS' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83204 WHERE slot = 'EQUIPMENT_SLOT_WAIST' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83186 WHERE slot = 'EQUIPMENT_SLOT_LEGS' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83195 WHERE slot = 'EQUIPMENT_SLOT_FEET' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83235 WHERE slot = 'EQUIPMENT_SLOT_FINGER1' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83236 WHERE slot = 'EQUIPMENT_SLOT_FINGER2' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83245 WHERE slot = 'EQUIPMENT_SLOT_TRINKET1' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83247 WHERE slot = 'EQUIPMENT_SLOT_TRINKET2' AND `specialization` = '270';
UPDATE `battle_pay_boost_items` SET itemId = 83260 WHERE slot = 'EQUIPMENT_SLOT_MAINHAND' AND `specialization` = '270';
DELETE FROM `battle_pay_boost_items` WHERE `specialization` = '270' AND `slot` = 'EQUIPMENT_SLOT_OFFHAND';