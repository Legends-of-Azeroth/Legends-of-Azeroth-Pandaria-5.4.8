/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRINITY_DBCSFRM_H
#define TRINITY_DBCSFRM_H

// x - skip<uint32>, X - skip<uint8>, s - char*, S - char*, f - float, i - uint32, b - uint8, d - index (not included)
// n - index (included), l - bool, p - field present in sql dbc, a - field absent in sql dbc

char const Achievementfmt[] =            "niiisxiixixxiii";
const std::string CustomAchievementfmt = "pppaaaapapaapppa";
const std::string CustomAchievementIndex = "ID";
char const AchievementCriteriafmt[] = "niiiixiiiisiiiiixxiiiii";
char const AnimKitfmt[] = "nxx";
char const AreaTableEntryfmt[] = "niiiixxxxxxxisiiiiiffixxxxxxxx";
char const AreaGroupEntryfmt[] = "niiiiiii";
char const AreaPOIEntryfmt[] = "niiiiiiiiiiiffixixxixxx";
char const AreaTriggerEntryfmt[] = "nifffiiifffffiii";
char const ArmorLocationfmt[] = "nfffff";
char const AuctionHouseEntryfmt[] = "niiix";
char const BankBagSlotPricesEntryfmt[] = "ni";
char const BannedAddOnsfmt[] = "nxxxxxxxxxx";
char const BarberShopStyleEntryfmt[] = "nixxxiii";
char const BattlemasterListEntryfmt[] = "niiiiiiiiiiiiiiiiixsiiiixxxxxx";
char const CharStartOutfitEntryfmt[] = "dbbbXiiiiiiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxii";
char const CharTitlesEntryfmt[] = "nisxix";
char const ChatChannelsEntryfmt[] = "nixss";
char const ChrClassesEntryfmt[] = "nixsxxxixiiiixxxxx";
char const ChrRacesEntryfmt[] = "nxixiixixxxxixsxxxxxxxxxxxxxxxxxxxxx";
char const ChrClassesXPowerTypesfmt[] = "nii";
char const CinematicCameraEntryfmt[] = "nSiffff";
char const CinematicSequencesEntryfmt[] = "nxiiiiiiii";
char const CreatureDisplayInfofmt[] = "nixifxxxxxxxxxxxxxxx";
char const CreatureModelDatafmt[] = "niSxfxxxxxxxxxxffxxxxxxxxxxxxxxxxx";
char const CreatureFamilyfmt[] = "nfifiiiiixsx";
char const CreatureImmunitiesfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiii";
char const CreatureSpellDatafmt[] = "niiiixxxx";
char const CreatureTypefmt[] = "nxx";
char const CurrencyTypesfmt[] = "nisxxiiiiixx";
char const Criteriafmt[] = "niiiiiiiiiii";
char const CriteriaTreefmt[] = "niixiiis";
char const ChrSpecializationfmt[] = "nxiixiiixxxsxx";
char const DestructibleModelDatafmt[] = "nixxxixxxxixxxxixxxxixxx";
char const DungeonEncounterfmt[] = "niixisxxx";
char const DurabilityCostsfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiii";
char const DurabilityQualityfmt[] = "nf";
char const EmotesEntryfmt[] = "nxxiiixx";
char const EmotesTextEntryfmt[] = "nxixxxxxxxxxxxxxxxx";
char const FactionEntryfmt[] = "niiiiiiiiiiiiiiiiiiffixsxixx";
char const FactionTemplateEntryfmt[] = "niiiiiiiiiiiii";
char const GameObjectDisplayInfofmt[] = "nsxxxxxxxxxxffffffxxx";
char const GemPropertiesEntryfmt[] = "nixxii";
char const GlyphPropertiesfmt[] = "niii";
char const GlyphSlotfmt[] = "nii";
char const GtBarberShopCostBasefmt[] = "xf";
char const GtBattlePetTypeDamageModfmt[] = "xf";
char const GtBattlePetXpfmt[] = "xf";
char const GtCombatRatingsfmt[] = "xf";
char const GtOCTHpPerStaminafmt[] = "df";
char const GtChanceToMeleeCritBasefmt[] = "xf";
char const GtChanceToMeleeCritfmt[] = "xf";
char const GtChanceToSpellCritBasefmt[] = "xf";
char const GtChanceToSpellCritfmt[] = "xf";
char const GtItemSocketCostPerLevelfmt[] = "xf";
char const GtNPCManaCostScalerfmt[] = "xf";
char const GtOCTClassCombatRatingScalarfmt[] = "df";
char const GtRegenMPPerSptfmt[] = "xf";
char const GtSpellScalingfmt[] = "df";
char const GtOCTBaseHPByClassfmt[] = "df";
char const GtOCTBaseMPByClassfmt[] = "df";
char const GuildPerkSpellsfmt[] = "dii";
char const Holidaysfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiixxsiix";
char const ImportPriceArmorfmt[] = "nffff";
char const ImportPriceQualityfmt[] = "nf";
char const ImportPriceShieldfmt[] = "nf";
char const ImportPriceWeaponfmt[] = "nf";
char const ItemPriceBasefmt[] = "diff";
char const ItemReforgefmt[] = "nifif";
char const ItemBagFamilyfmt[] = "nx";
char const ItemArmorQualityfmt[] = "nfffffffi";
char const ItemArmorShieldfmt[] = "nifffffff";
char const ItemArmorTotalfmt[] = "niffff";
char const ItemClassfmt[] = "difx";
char const ItemDamagefmt[] = "nfffffffi";
char const ItemDisenchantLootfmt[] = "niiiiii";
char const ItemDisplayTemplateEntryfmt[] = "nxxxxsxxxxxxxxxxxxxxxxxxxx";
char const ItemLimitCategoryEntryfmt[] = "nxii";
char const ItemNameDescriptionfmt[] = "nsi";
char const ItemRandomPropertiesfmt[] = "nxiiixxs";
char const ItemRandomSuffixfmt[] = "nsxiiiiiiiiii";
char const ItemSetEntryfmt[] = "dsiiiiiiiiiixxxxxxxiiiiiiiiiiiiiiiiii";
char const ItemSpecEntryfmt[] = "xiiiiii";
char const ItemSpecOverrideEntryfmt[] = "xii";
char const LFGDungeonEntryfmt[] = "nsiiiiiiiiiixixixiiiixxxiixxx";
char const LightEntryfmt[] = "nifffxxxxxxxxxx";
char const LiquidTypefmt[] = "nxxixixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char const LockEntryfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiixxxxxxxx";
char const PhaseEntryfmt[] = "nsi";
char const PhaseGroupfmt[] = "nii";
char const MailTemplateEntryfmt[] = "nss";
char const MapEntryfmt[] = "nxixxsixxixiffxiiii";
char const MapDifficultyEntryfmt[] = "diisiix";
char const ModifierTreefmt[] = "niiiixi";
char const MovieEntryfmt[] = "nxxxx";
char const MountCapabilityfmt[] = "niiiiiii";
char const MountTypefmt[] = "niiiiiiiiiiiiiiiiiiiiiiii";
char const NameGenfmt[] = "dsii";
char const NumTalentsAtLevelfmt[] = "df";
char const PlayerConditionEntryfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char const OverrideSpellDatafmt[] = "niiiiiiiiiixx";
char const QuestFactionRewardfmt[] = "niiiiiiiiii";
char const QuestSortEntryfmt[] = "nx";
char const QuestV2fmt[] = "ni";
char const QuestXPfmt[] = "niiiiiiiiii";
char const QuestPOIPointfmt [] = "diii";
char const PvPDifficultyfmt[] = "diiiii";
char const RandomPropertiesPointsfmt[] = "niiiiiiiiiiiiiii";
char const ResearchBranchfmt [] = "nxxixi";
char const ResearchProjectfmt [] = "nxxiiiixi";
char const ResearchSitefmt [] = "niisx";
char const ScalingStatDistributionfmt[] = "niiiiiiiiiiiiiiiiiiiixi";
char const ScalingStatValuesfmt[] = "iniiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
char const Scenariofmt[] = "nxi";
char const ScenarioStepfmt[] = "niiixxi";
char const SkillRaceClassInfofmt[] = "niiiiiii";
char const SkillTiersfmt[] = "niiiiiiiiiiiiiiii";
char const SkillLinefmt[] = "nissixixx";
char const SkillLineAbilityfmt[] = "niiiiiiiiiiix";
char const SoundEntriesfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char const SpecializationSpellsfmt[] = "niiix";
char const SpellCastTimefmt[] = "nixx";
char const SpellCategoriesEntryfmt[] = "niiiiiiiii";
char const SpellCategoryEntryfmt[] = "niisii";
char const SpellDifficultyfmt[] = "niiii";
const std::string CustomSpellDifficultyfmt = "ppppp";
const std::string CustomSpellDifficultyIndex = "id";
char const SpellDurationfmt[] = "niii";
char const SpellEffectEntryfmt[] =            "niifiiiffiiiiiifiifiiiiixiiiix";
const std::string CustomSpellEffectEntryfmt = "papppppppppapppappppppppappppp";
const std::string CustomSpellEffectEntryIndex = "Id";
char const SpellEntryfmt[] =            "nssxxixxiiiiiiiiiiiiiiiii";
const std::string CustomSpellEntryfmt = "paaaaaaaaapapppapppaapaapa";
const std::string CustomSpellEntryIndex = "Id";
char const SpellMiscfmt[]            = "nxxiiiiiiiiiiiiiiiiifiiiii";
const std::string CustomSpellMiscfmt = "paapppppppppppppppppaaaaapa";
const std::string CustomSpellMiscIndex = "Id";
char const SpellEffectScalingfmt[] = "nfffxi";
char const SpellFocusObjectfmt[] = "nx";
char const SpellItemEnchantmentfmt[] = "niiiiiiiiiisiiiiiiiixixfff";
char const SpellItemEnchantmentConditionfmt[] = "nbbbbbxxxxxbbbbbbbbbbiiiiiXXXXX";
char const SpellRadiusfmt[] = "nffxf";
char const SpellRangefmt[] = "nffffixx";
char const SpellScalingEntryfmt[] = "diiiifiii";
char const SpellTotemsEntryfmt[] = "niiii";
char const SpellTargetRestrictionsEntryfmt[] = "niifxiiii";
char const SpellPowerEntryfmt[] = "nixiiiixxffix";
char const SpellInterruptsEntryfmt[] = "dxxiiiii";
char const SpellEquippedItemsEntryfmt[] = "dxxiii";
char const SpellAuraOptionsEntryfmt[] = "nxxiiiiii";
char const SpellAuraRestrictionsEntryfmt[] = "dxxiiiiiiii";
char const SpellCastingRequirementsEntryfmt[] = "dixxixi";
char const SpellClassOptionsEntryfmt[] = "dxiiiii";
char const SpellCooldownsEntryfmt[] = "dxxiii";
char const SpellLevelsEntryfmt[] = "dxxiii";
char const SpellProcsPerMinutefmt[] = "nii";
char const SpellProcsPerMinuteModfmt[] = "niifi";
char const SpellRuneCostfmt[] = "niiiii";
char const SpellShapeshiftEntryfmt[] = "nixixx";
char const SpellShapeshiftFormfmt[] = "nxxiixiiixxiiiiiiiixx";
char const SummonPropertiesfmt[] = "niiiii";
char const TalentEntryfmt[] = "nxiiixxxiix";
char const TaxiNodesEntryfmt[] = "nifffsiixixx";
char const TaxiPathEntryfmt[] = "niii";
char const TaxiPathNodeEntryfmt[] = "diiifffiiii";
char const TotemCategoryEntryfmt[] = "nxii";
char const UnitPowerBarfmt[] = "niixxxxxxxxxxxxxxxxxxxxxxxx";
char const TransportAnimationfmt[] = "diifffx";
char const TransportRotationfmt[] = "diiffff";
char const VehicleEntryfmt[] = "nixffffiiiiiiiifffffffffffffffssssfifiixx";
char const VehicleSeatEntryfmt[] = "niiffffffffffiiiiiifffffffiiifffiiiiiiiffiiiixxxxxxxxxxxxxxxxxxxxx";
char const WMOAreaTableEntryfmt[] = "niiixxxxxiixxxx";
char const WorldMapAreaEntryfmt[] = "xinxffffixxxxx";
char const WorldMapOverlayEntryfmt[] = "nxiiiixxxxxxxxxx";
char const WorldSafeLocsEntryfmt[] = "niffffx";

#endif
