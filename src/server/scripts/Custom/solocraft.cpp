
/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include <map>
#include "Config.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Pet.h"
#include "Map.h"
#include "Group.h"
#include "InstanceScript.h"
#include "Chat.h"
#include "Log.h"
#include <math.h>
#include <unordered_map>
#include <unordered_set>

enum SolocraftTrinityStrings 
{
    SOLOCRAFT_TRINITYSTRING_ACTIVE                       = 30000,    // |cff4CFF00SoloCraft system|r active.
    SOLOCRAFT_TRINITYSTRING_STATUS                       = 30001,    // |cffFF0000[SoloCraft]|r |cffFF8000 %s entered %s - Difficulty Offset: %0.2f. Spellpower Bonus: %i. Class Balance Weight: %i. XP Gain: |cffFF0000%s XP Balancing:%s |cff4CFF00%s
    SOLOCRAFT_TRINITYSTRING_ERR_LEVEL_TOO_HIGH           = 30002,    // |cff4CFF00[SoloCraft]|r |cffFF0000 %s entered %s - |cffFF0000You have not been buffed.|r |cffFF8000Your level is higher than the max level (%i) threshold for this dungeon.
    SOLOCRAFT_TRINITYSTRING_ERR_GROUP_ALREADY_BUFFED     = 30003,    // |cffFF0000[SoloCraft]|r |cffFF8000 %s entered %s - |cffFF0000BE ADVISED - You have been debuffed by offset: %0.2f with a Class Balance Weight: %i. |cffFF8000A group member already inside has the dungeon's full buff offset. No Spellpower buff will be applied to spell casters. ALL group members must exit the dungeon and re-enter to receive a balanced offset.
    SOLOCRAFT_TRINITYSTRING_CLEAR_BUFFS                  = 30004,    // |cffFF0000[SoloCraft]|r |cffFF8000 %s exited to %s - Reverting Difficulty Offset: %0.2f. Spellpower Bonus Removed: %i 
    SOLOCRAFT_TRINITYSTRING_ENABLED                      = 30005,    // Enabled
    SOLOCRAFT_TRINITYSTRING_DISABLED                     = 30006,    // Disabled
};

class SolocraftConfig
{
    public:

        SolocraftConfig() 
        { 
            loadConfig();
        }

        static SolocraftConfig &getInstance() {
            static SolocraftConfig instance;
            return instance;
        }        
        
        void loadConfig()
        {
            SoloCraftEnable = sConfigMgr->GetBoolDefault("Solocraft.Enable", 0);
            SoloCraftAnnounceModule = sConfigMgr->GetBoolDefault("Solocraft.Announce", 1);
            SoloCraftDebuffEnable = sConfigMgr->GetBoolDefault("SoloCraft.Debuff.Enable", 1);
            SoloCraftSpellMult = sConfigMgr->GetFloatDefault("SoloCraft.Spellpower.Mult", 2.5);
            SoloCraftStatsMult = sConfigMgr->GetFloatDefault("SoloCraft.Stats.Mult", 100.0);

            classes =
            {
                {1, sConfigMgr->GetIntDefault("SoloCraft.Warrior", 100) },
                {2, sConfigMgr->GetIntDefault("SoloCraft.Paladin", 100) },
                {3, sConfigMgr->GetIntDefault("SoloCraft.Hunter", 100) },
                {4, sConfigMgr->GetIntDefault("SoloCraft.Rogue", 100) },
                {5, sConfigMgr->GetIntDefault("SoloCraft.Priest", 100) },
                {6, sConfigMgr->GetIntDefault("SoloCraft.Death.Knight", 100) },
                {7, sConfigMgr->GetIntDefault("SoloCraft.Shaman", 100) },
                {8, sConfigMgr->GetIntDefault("SoloCraft.Mage", 100) },
                {9, sConfigMgr->GetIntDefault("SoloCraft.Warlock", 100) },
                {10, sConfigMgr->GetIntDefault("SoloCraft.Monk", 100) },
                {11, sConfigMgr->GetIntDefault("SoloCraft.Druid", 100) },
            };

            SolocraftXPEnabled = sConfigMgr->GetBoolDefault("Solocraft.XP.Enabled", 1);
            SolocraftXPBalancingEnabled = sConfigMgr->GetBoolDefault("Solocraft.XP.Balancing.Enabled", 1);
            SoloCraftXPMod = sConfigMgr->GetFloatDefault("Solocraft.XP.Mod", 1.0);
            SolocraftNoXPFlag = sConfigMgr->GetBoolDefault("Solocraft.NoXP.Keep", 0);
            if (SoloCraftXPMod < 0.0f)
            {
                SoloCraftXPMod = 0.0f;
            }
            SolocraftLevelDiff = sConfigMgr->GetIntDefault("Solocraft.Max.Level.Diff", 10);
            SolocraftDungeonLevel = sConfigMgr->GetIntDefault("Solocraft.Dungeon.Level", 90);

            dungeons =
            {
                /// VANILLA
                {34, sConfigMgr->GetIntDefault("Solocraft.Stockades.Level", 22) },
                {43, sConfigMgr->GetIntDefault("Solocraft.WailingCaverns.Level", 17) },
                {47, sConfigMgr->GetIntDefault("Solocraft.RazorfenKraul.Level", 30) },
                {48, sConfigMgr->GetIntDefault("Solocraft.BlackfathomDeeps.Level", 20) },
                {70, sConfigMgr->GetIntDefault("Solocraft.Uldaman.Level", 40) },
                {90, sConfigMgr->GetIntDefault("Solocraft.Gnomeregan.Level", 24) },
                {109, sConfigMgr->GetIntDefault("Solocraft.SunkenTemple.Level", 50) },
                {129, sConfigMgr->GetIntDefault("Solocraft.RazorfenDowns.Level", 40) },
                {209, sConfigMgr->GetIntDefault("Solocraft.ZulFarrak.Level", 44) },
                {229, sConfigMgr->GetIntDefault("Solocraft.BlackRockSpire.Level", 55) },
                {230, sConfigMgr->GetIntDefault("Solocraft.BlackrockDepths.Level", 50) },
                {249, sConfigMgr->GetIntDefault("Solocraft.OnyxiaLair.Level", 60) },
                {329, sConfigMgr->GetIntDefault("Solocraft.Stratholme.Level", 55) },
                {349, sConfigMgr->GetIntDefault("Solocraft.Mauradon.Level", 48) },
                {389, sConfigMgr->GetIntDefault("Solocraft.RagefireChasm.Level", 15) },
                {409, sConfigMgr->GetIntDefault("Solocraft.MoltenCore.Level", 60) },
                {429, sConfigMgr->GetIntDefault("Solocraft.DireMaul.Level", 48) },
                {469, sConfigMgr->GetIntDefault("Solocraft.BlackwingLair.Level", 40) },
                {509, sConfigMgr->GetIntDefault("Solocraft.RuinsOfAhnQiraj.Level", 60) },
                {531, sConfigMgr->GetIntDefault("Solocraft.TempleOfAhnQiraj.Level", 60) },
                /// BURNING CRUSADE
                {269, sConfigMgr->GetIntDefault("Solocraft.TheBlackMorass.Level", 68) },
                {532, sConfigMgr->GetIntDefault("Solocraft.Karazahn.Level", 68) },
                {534, sConfigMgr->GetIntDefault("Solocraft.TheBattleForMountHyjal.Level", 70) },
                {540, sConfigMgr->GetIntDefault("Solocraft.TheShatteredHalls.Level", 68) },
                {542, sConfigMgr->GetIntDefault("Solocraft.TheBloodFurnace.Level", 68) },
                {543, sConfigMgr->GetIntDefault("Solocraft.HellfireRampart.Level", 68) },
                {544, sConfigMgr->GetIntDefault("Solocraft.MagtheridonsLair.Level", 68) },
                {545, sConfigMgr->GetIntDefault("Solocraft.TheSteamVault.Level", 68) },
                {546, sConfigMgr->GetIntDefault("Solocraft.TheUnderbog.Level", 68) },
                {547, sConfigMgr->GetIntDefault("Solocraft.TheSlavePens.Level", 68) },
                {548, sConfigMgr->GetIntDefault("Solocraft.SerpentshrineCavern.Level", 70) },
                {550, sConfigMgr->GetIntDefault("Solocraft.TheEye.Level", 70) },
                {552, sConfigMgr->GetIntDefault("Solocraft.TheArcatraz.Level", 68) },
                {553, sConfigMgr->GetIntDefault("Solocraft.TheBotanica.Level", 68) },
                {554, sConfigMgr->GetIntDefault("Solocraft.TheMechanar.Level", 68) },
                {555, sConfigMgr->GetIntDefault("Solocraft.ShadowLabyrinth.Level", 68) },
                {556, sConfigMgr->GetIntDefault("Solocraft.SethekkHalls.Level", 68) },
                {557, sConfigMgr->GetIntDefault("Solocraft.ManaTombs.Level", 68) },
                {558, sConfigMgr->GetIntDefault("Solocraft.AuchenaiCrypts.Level", 68) },
                {560, sConfigMgr->GetIntDefault("Solocraft.OldHillsbradFoothills.Level", 68) },
                {564, sConfigMgr->GetIntDefault("Solocraft.BlackTemple.Level", 70) },
                {565, sConfigMgr->GetIntDefault("Solocraft.GruulsLair.Level", 70) },
                {580, sConfigMgr->GetIntDefault("Solocraft.SunwellPlateau.Level", 70) },
                {585, sConfigMgr->GetIntDefault("Solocraft.MagistersTerrace.Level", 68) },
                /// WRATH OF THE LICH KING
                {533, sConfigMgr->GetIntDefault("Solocraft.Naxxramas.Level", 78) },
                {574, sConfigMgr->GetIntDefault("Solocraft.UtgardeKeep.Level", 78) },
                {575, sConfigMgr->GetIntDefault("Solocraft.UtgardePinnacle.Level", 78) },
                {578, sConfigMgr->GetIntDefault("Solocraft.Oculus.Level", 78) },
                {595, sConfigMgr->GetIntDefault("Solocraft.TheCullingOfStratholme.Level", 78) },
                {599, sConfigMgr->GetIntDefault("Solocraft.HallsOfStone.Level", 78) },
                {600, sConfigMgr->GetIntDefault("Solocraft.DrakTharonKeep.Level", 78) },
                {601, sConfigMgr->GetIntDefault("Solocraft.AzjolNerub.Level", 78) },
                {602, sConfigMgr->GetIntDefault("Solocraft.HallsOfLighting.Level", 78) },
                {603, sConfigMgr->GetIntDefault("Solocraft.Ulduar.Level", 80) },
                {604, sConfigMgr->GetIntDefault("Solocraft.GunDrak.Level", 78) },
                {608, sConfigMgr->GetIntDefault("Solocraft.VioletHold.Level", 78) },
                {615, sConfigMgr->GetIntDefault("Solocraft.TheObsidianSanctum.Level", 80) },
                {616, sConfigMgr->GetIntDefault("Solocraft.TheEyeOfEternity.Level", 80) },
                {619, sConfigMgr->GetIntDefault("Solocraft.AhnkahetTheOldKingdom.Level", 78) },
                {631, sConfigMgr->GetIntDefault("Solocraft.IcecrownCitadel.Level", 80) },
                {632, sConfigMgr->GetIntDefault("Solocraft.TheForgeOfSouls.Level", 78) },
                {649, sConfigMgr->GetIntDefault("Solocraft.TrialOfTheCrusader.Level", 80) },
                {650, sConfigMgr->GetIntDefault("Solocraft.TrialOfTheChampion.Level", 80) },
                {658, sConfigMgr->GetIntDefault("Solocraft.PitOfSaron.Level", 78) },
                {668, sConfigMgr->GetIntDefault("Solocraft.HallsOfReflection.Level", 78) },
                {724, sConfigMgr->GetIntDefault("Solocraft.TheRubySanctum.Level", 80) },
                /// CATACLYSM
                {33, sConfigMgr->GetIntDefault("Solocraft.ShadowfangKeep.Level", 85) }, 
                {36, sConfigMgr->GetIntDefault("Solocraft.DeadMines.Level", 85) }, 
                {645, sConfigMgr->GetIntDefault("Solocraft.BlackrockCaverns.Level", 85) }, 
                {643, sConfigMgr->GetIntDefault("Solocraft.ThroneOfTheTides.Level", 85) }, 
                {657, sConfigMgr->GetIntDefault("Solocraft.TheVortexPinnacle.Level", 85) }, 
                {725, sConfigMgr->GetIntDefault("Solocraft.TheStonecore.Level", 85) }, 
                {755, sConfigMgr->GetIntDefault("Solocraft.LostCityOfTheTol'vir.Level", 85) }, 
                {644, sConfigMgr->GetIntDefault("Solocraft.HallsOfOrigination.Level", 85) }, 
                {670, sConfigMgr->GetIntDefault("Solocraft.GrimBatol.Level", 85) }, 
                {669, sConfigMgr->GetIntDefault("Solocraft.BlackwingDescent.Level", 85) }, 
                {671, sConfigMgr->GetIntDefault("Solocraft.TheBastionOfTwilight.Level", 85) }, 
                {754, sConfigMgr->GetIntDefault("Solocraft.ThroneOfTheFourWinds.Level", 85) }, 
                {757, sConfigMgr->GetIntDefault("Solocraft.BaradinHold.Level", 85) }, 
                {720, sConfigMgr->GetIntDefault("Solocraft.Firelands.Level", 85) }, 
                {967, sConfigMgr->GetIntDefault("Solocraft.DragonSoul.Level", 85) }, 
                {938, sConfigMgr->GetIntDefault("Solocraft.EndTime.Level", 85) }, 
                {939, sConfigMgr->GetIntDefault("Solocraft.WellOfEternity.Level", 85) }, 
                {940, sConfigMgr->GetIntDefault("Solocraft.HourOfTwilight.Level", 85) }, 
                {859, sConfigMgr->GetIntDefault("Solocraft.Zul'gurub.Level", 85) }, 
                {568, sConfigMgr->GetIntDefault("Solocraft.ZulAman.Level", 85) }, 
                {576, sConfigMgr->GetIntDefault("Solocraft.Nexus.Level", 85) }, 
                /// MISTS OF PANDARIA
                {959, sConfigMgr->GetIntDefault("Solocraft.ShadoPanMonastery.Level", 90) }, 
                {1007, sConfigMgr->GetIntDefault("Solocraft.Scholomance.Level", 90) }, 
                {1004, sConfigMgr->GetIntDefault("Solocraft.ScarletMonastery.Level", 90) }, 
                {994, sConfigMgr->GetIntDefault("Solocraft.Mogu'shanPalace.Level", 90) }, 
                {1008, sConfigMgr->GetIntDefault("Solocraft.Mogu'shanVaults.Level", 90) }, 
                {1136, sConfigMgr->GetIntDefault("Solocraft.SiegeOfOrgrimmar.Level", 90) }, 
                {1098, sConfigMgr->GetIntDefault("Solocraft.ThroneOfThunder.Level", 90) }, 
                {1009, sConfigMgr->GetIntDefault("Solocraft.HeartOfFear.Level", 90) }, 
                {996, sConfigMgr->GetIntDefault("Solocraft.TerraceOfEndlessSpring.Level", 90) }, 
                {1001, sConfigMgr->GetIntDefault("Solocraft.ScarletHalls.Level", 90) }, 
                {962, sConfigMgr->GetIntDefault("Solocraft.GateOfTheSettingSun.Level", 90) }, 
                {1011, sConfigMgr->GetIntDefault("Solocraft.SiegeOfNiuzaoTemple.Level", 90) }, 
                {960, sConfigMgr->GetIntDefault("Solocraft.TempleOfTheJadeSerpent.Level", 90) }, 
                {961, sConfigMgr->GetIntDefault("Solocraft.StormstoutBrewery.Level", 90) }, 
            };

            // Dungeon difficulty
            D5 = sConfigMgr->GetFloatDefault("Solocraft.Dungeon", 5.0);
            D10 = sConfigMgr->GetFloatDefault("Solocraft.Heroic", 10.0);
            D25 = sConfigMgr->GetFloatDefault("Solocraft.Raid25", 25.0);
            D40 = sConfigMgr->GetFloatDefault("Solocraft.Raid40", 40.0);
            // Множитель (обычный)
            diff_Multiplier =
            {
                /// VANILLA
                {34, sConfigMgr->GetFloatDefault("Solocraft.Stockades", 5.0) },
                {43, sConfigMgr->GetFloatDefault("Solocraft.WailingCaverns", 5.0) },
                {47, sConfigMgr->GetFloatDefault("Solocraft.RazorfenKraul", 5.0) },
                {48, sConfigMgr->GetFloatDefault("Solocraft.BlackfathomDeeps", 5.0) },
                {70, sConfigMgr->GetFloatDefault("Solocraft.Uldaman", 5.0) },
                {90, sConfigMgr->GetFloatDefault("Solocraft.Gnomeregan", 5.0) },
                {109, sConfigMgr->GetFloatDefault("Solocraft.SunkenTemple", 5.0) },
                {129, sConfigMgr->GetFloatDefault("Solocraft.RazorfenDowns", 5.0) },
                {209, sConfigMgr->GetFloatDefault("Solocraft.ZulFarrak", 5.0) },
                {229, sConfigMgr->GetFloatDefault("Solocraft.BlackRockSpire", 10.0) },
                {230, sConfigMgr->GetFloatDefault("Solocraft.BlackrockDepths", 5.0) },
                {249, sConfigMgr->GetFloatDefault("Solocraft.OnyxiaLair", 40.0) },
                {329, sConfigMgr->GetFloatDefault("Solocraft.Stratholme", 5.0) },
                {349, sConfigMgr->GetFloatDefault("Solocraft.Mauradon", 5.0) },
                {389, sConfigMgr->GetFloatDefault("Solocraft.RagefireChasm", 5.0) },
                {409, sConfigMgr->GetFloatDefault("Solocraft.MoltenCore", 40.0) },
                {429, sConfigMgr->GetFloatDefault("Solocraft.DireMaul", 5.0) },
                {469, sConfigMgr->GetFloatDefault("Solocraft.BlackwingLair", 40.0) },
                {509, sConfigMgr->GetFloatDefault("Solocraft.RuinsOfAhnQiraj", 20.0) },
                {531, sConfigMgr->GetFloatDefault("Solocraft.TempleOfAhnQiraj", 40.0) },
                /// BURNING CRUSADE
                {269, sConfigMgr->GetFloatDefault("Solocraft.TheBlackMorass", 5.0) },
                {532, sConfigMgr->GetFloatDefault("Solocraft.Karazahn", 10.0) },
                {534, sConfigMgr->GetFloatDefault("Solocraft.TheBattleForMountHyjal", 25.0) },
                {540, sConfigMgr->GetFloatDefault("Solocraft.TheShatteredHalls", 5.0) },
                {542, sConfigMgr->GetFloatDefault("Solocraft.TheBloodFurnace", 5.0) },
                {543, sConfigMgr->GetFloatDefault("Solocraft.HellfireRampart", 5.0) },
                {544, sConfigMgr->GetFloatDefault("Solocraft.MagtheridonsLair", 25.0) },
                {545, sConfigMgr->GetFloatDefault("Solocraft.TheSteamVault", 5.0) },
                {546, sConfigMgr->GetFloatDefault("Solocraft.TheUnderbog", 5.0) },
                {547, sConfigMgr->GetFloatDefault("Solocraft.TheSlavePens", 5.0) },
                {548, sConfigMgr->GetFloatDefault("Solocraft.SerpentshrineCavern", 25.0) },
                {550, sConfigMgr->GetFloatDefault("Solocraft.TheEye", 25.0) },
                {552, sConfigMgr->GetFloatDefault("Solocraft.TheArcatraz", 5.0) },
                {553, sConfigMgr->GetFloatDefault("Solocraft.TheBotanica", 5.0) },
                {554, sConfigMgr->GetFloatDefault("Solocraft.TheMechanar", 5.0) },
                {555, sConfigMgr->GetFloatDefault("Solocraft.ShadowLabyrinth", 5.0) },
                {556, sConfigMgr->GetFloatDefault("Solocraft.SethekkHalls", 5.0) },
                {557, sConfigMgr->GetFloatDefault("Solocraft.ManaTombs", 5.0) },
                {558, sConfigMgr->GetFloatDefault("Solocraft.AuchenaiCrypts", 5.0) },
                {560, sConfigMgr->GetFloatDefault("Solocraft.OldHillsbradFoothills", 5.0) },
                {564, sConfigMgr->GetFloatDefault("Solocraft.BlackTemple", 25.0) },
                {565, sConfigMgr->GetFloatDefault("Solocraft.GruulsLair", 25.0) },
                {580, sConfigMgr->GetFloatDefault("Solocraft.SunwellPlateau", 25.0) },
                {585, sConfigMgr->GetFloatDefault("Solocraft.MagistersTerrace", 5.0) },
                /// WRATH OF THE LICH KING
                {533, sConfigMgr->GetFloatDefault("Solocraft.Naxxramas", 10.0) },
                {574, sConfigMgr->GetFloatDefault("Solocraft.UtgardeKeep", 5.0) },
                {575, sConfigMgr->GetFloatDefault("Solocraft.UtgardePinnacle", 5.0) },
                {578, sConfigMgr->GetFloatDefault("Solocraft.Oculus", 5.0) },
                {595, sConfigMgr->GetFloatDefault("Solocraft.TheCullingOfStratholme", 5.0) },
                {599, sConfigMgr->GetFloatDefault("Solocraft.HallsOfStone", 5.0) },
                {600, sConfigMgr->GetFloatDefault("Solocraft.DrakTharonKeep", 5.0) },
                {601, sConfigMgr->GetFloatDefault("Solocraft.AzjolNerub", 5.0) },
                {602, sConfigMgr->GetFloatDefault("Solocraft.HallsOfLighting", 5.0) },
                {603, sConfigMgr->GetFloatDefault("Solocraft.Ulduar", 10.0) },
                {604, sConfigMgr->GetFloatDefault("Solocraft.GunDrak", 5.0) },
                {608, sConfigMgr->GetFloatDefault("Solocraft.VioletHold", 5.0) },
                {615, sConfigMgr->GetFloatDefault("Solocraft.TheObsidianSanctum", 10.0) },
                {616, sConfigMgr->GetFloatDefault("Solocraft.TheEyeOfEternity", 10.0) },
                {619, sConfigMgr->GetFloatDefault("Solocraft.AhnkahetTheOldKingdom", 5.0) },
                {631, sConfigMgr->GetFloatDefault("Solocraft.IcecrownCitadel", 10.0) },
                {632, sConfigMgr->GetFloatDefault("Solocraft.TheForgeOfSouls", 5.0) },
                {649, sConfigMgr->GetFloatDefault("Solocraft.TrialOfTheCrusader", 10.0) },
                {650, sConfigMgr->GetFloatDefault("Solocraft.TrialOfTheChampion", 5.0) },
                {658, sConfigMgr->GetFloatDefault("Solocraft.PitOfSaron", 5.0) },
                {668, sConfigMgr->GetFloatDefault("Solocraft.HallsOfReflection", 5.0) },
                {724, sConfigMgr->GetFloatDefault("Solocraft.TheRubySanctum", 10.0) },
                /// CATACLYSM
                {33, sConfigMgr->GetFloatDefault("Solocraft.ShadowfangKeep", 5.0) }, // ShadowfangKeep
                {36, sConfigMgr->GetFloatDefault("Solocraft.DeadMines", 5.0) }, // DeadMines
                {645, sConfigMgr->GetFloatDefault("Solocraft.BlackrockCaverns", 5.0) }, // BlackrockCaverns
                {643, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfTheTides", 5.0) }, // Трон Приливов
                {657, sConfigMgr->GetFloatDefault("Solocraft.TheVortexPinnacle", 5.0) }, // Вершина Смерча
                {725, sConfigMgr->GetFloatDefault("Solocraft.TheStonecore", 5.0) }, // Каменные Недра
                {755, sConfigMgr->GetFloatDefault("Solocraft.LostCityOfTheTol'vir", 5.0) }, // Затерянный город Тол'вир
                {644, sConfigMgr->GetFloatDefault("Solocraft.HallsOfOrigination", 5.0) }, // Чертоги Созидания
                {670, sConfigMgr->GetFloatDefault("Solocraft.GrimBatol", 5.0) }, // Грим Батол
                {669, sConfigMgr->GetFloatDefault("Solocraft.BlackwingDescent", 10.0) }, // Твердыня Крыла Тьмы
                {671, sConfigMgr->GetFloatDefault("Solocraft.TheBastionOfTwilight", 10.0) }, // Сумеречный бастион
                {754, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfTheFourWinds", 10.0) }, // Трон Четырех Ветров
                {757, sConfigMgr->GetFloatDefault("Solocraft.BaradinHold", 10.0) }, // Крепость Барадин
                {720, sConfigMgr->GetFloatDefault("Solocraft.Firelands", 10.0) }, // Огненные Просторы
                {967, sConfigMgr->GetFloatDefault("Solocraft.DragonSoul", 10.0) }, // Душа Дракона
                {859, sConfigMgr->GetFloatDefault("Solocraft.Zul'gurub", 5.0) }, // Зул'Гуруб
                {568, sConfigMgr->GetFloatDefault("Solocraft.ZulAman", 5.0) }, // Зул'Аман
                {576, sConfigMgr->GetFloatDefault("Solocraft.Nexus", 5.0) }, // Нексус
                /// MISTS OF PANDARIA
                {959, sConfigMgr->GetFloatDefault("Solocraft.ShadoPanMonastery", 5.0) }, // Монастырь Шадо-Пан
                {1007, sConfigMgr->GetFloatDefault("Solocraft.Scholomance", 5.0) }, // Некроситет
                {1004, sConfigMgr->GetFloatDefault("Solocraft.ScarletMonastery", 5.0) }, // Монастырь Алого ордена
                {994, sConfigMgr->GetFloatDefault("Solocraft.Mogu'shanPalace", 5.0) }, // Дворец Могу'шан
                {1008, sConfigMgr->GetFloatDefault("Solocraft.Mogu'shanVaults", 10.0) }, // Подземелья Могу'шан
                {1136, sConfigMgr->GetFloatDefault("Solocraft.SiegeOfOrgrimmar", 10.0) }, // Осада Оргриммара
                {1098, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfThunder", 10.0) }, // Престол Гроз
                {1009, sConfigMgr->GetFloatDefault("Solocraft.HeartOfFear", 10.0) }, // Сердце Страха
                {996, sConfigMgr->GetFloatDefault("Solocraft.TerraceOfEndlessSpring", 10.0) }, // Терраса Вечной Весны
                {1001, sConfigMgr->GetFloatDefault("Solocraft.ScarletHalls", 5.0) }, // Залы Алого ордена
                {962, sConfigMgr->GetFloatDefault("Solocraft.GateOfTheSettingSun", 5.0) }, // Врата Заходящего Солнца
                {1011, sConfigMgr->GetFloatDefault("Solocraft.SiegeOfNiuzaoTemple", 5.0) }, // Осада храма Нюцзао
                {960, sConfigMgr->GetFloatDefault("Solocraft.TempleOfTheJadeSerpent", 5.0) }, // Храм Нефритовой Змеи
                {961, sConfigMgr->GetFloatDefault("Solocraft.StormstoutBrewery", 5.0) }, // Хмелеварня Буйных Портеров
            };
            // diff_Multiplier_Heroics
            diff_Multiplier_Heroics =
            {
                /// BURNING CRUSADE
                {269, sConfigMgr->GetFloatDefault("Solocraft.TheBlackMorassH", 5.0) },
                {540, sConfigMgr->GetFloatDefault("Solocraft.TheShatteredHallsH", 5.0) },
                {542, sConfigMgr->GetFloatDefault("Solocraft.TheBloodFurnaceH", 5.0) },
                {543, sConfigMgr->GetFloatDefault("Solocraft.HellfireRampartH", 5.0) },
                {545, sConfigMgr->GetFloatDefault("Solocraft.TheSteamVaultH", 5.0) },
                {546, sConfigMgr->GetFloatDefault("Solocraft.TheUnderbogH", 5.0) },
                {547, sConfigMgr->GetFloatDefault("Solocraft.TheSlavePensH", 5.0) },
                {552, sConfigMgr->GetFloatDefault("Solocraft.TheArcatrazH", 5.0) },
                {553, sConfigMgr->GetFloatDefault("Solocraft.TheBotanicaH", 5.0) },
                {554, sConfigMgr->GetFloatDefault("Solocraft.TheMechanarH", 5.0) },
                {555, sConfigMgr->GetFloatDefault("Solocraft.ShadowLabyrinthH", 5.0) },
                {556, sConfigMgr->GetFloatDefault("Solocraft.SethekkHallsH", 5.0) },
                {557, sConfigMgr->GetFloatDefault("Solocraft.ManaTombsH", 5.0) },
                {558, sConfigMgr->GetFloatDefault("Solocraft.AuchenaiCryptsH", 5.0) },
                {560, sConfigMgr->GetFloatDefault("Solocraft.OldHillsbradFoothillsH", 5.0) },
                {585, sConfigMgr->GetFloatDefault("Solocraft.MagistersTerraceH", 5.0) },
                /// WRATH OF THE LICH KING
                {533, sConfigMgr->GetFloatDefault("Solocraft.NaxxramasH", 25.0) },
                {574, sConfigMgr->GetFloatDefault("Solocraft.UtgardeKeepH", 5.0) },
                {575, sConfigMgr->GetFloatDefault("Solocraft.UtgardePinnacleH", 5.0) },
                {578, sConfigMgr->GetFloatDefault("Solocraft.OculusH", 5.0) },
                {595, sConfigMgr->GetFloatDefault("Solocraft.TheCullingOfStratholmeH", 5.0) },
                {599, sConfigMgr->GetFloatDefault("Solocraft.HallsOfStoneH", 5.0) },
                {600, sConfigMgr->GetFloatDefault("Solocraft.DrakTharonKeepH", 5.0) },
                {601, sConfigMgr->GetFloatDefault("Solocraft.AzjolNerubH", 5.0) },
                {602, sConfigMgr->GetFloatDefault("Solocraft.HallsOfLightingH", 5.0) },
                {603, sConfigMgr->GetFloatDefault("Solocraft.UlduarH", 25.0) },
                {604, sConfigMgr->GetFloatDefault("Solocraft.GunDrakH", 5.0) },
                {608, sConfigMgr->GetFloatDefault("Solocraft.VioletHoldH", 5.0) },
                {615, sConfigMgr->GetFloatDefault("Solocraft.TheObsidianSanctumH", 25.0) },
                {616, sConfigMgr->GetFloatDefault("Solocraft.TheEyeOfEternityH", 25.0) },
                {619, sConfigMgr->GetFloatDefault("Solocraft.AhnkahetTheOldKingdomH", 5.0) },
                {631, sConfigMgr->GetFloatDefault("Solocraft.IcecrownCitadelH", 25.0) },
                {632, sConfigMgr->GetFloatDefault("Solocraft.TheForgeOfSoulsH", 5.0) },
                {649, sConfigMgr->GetFloatDefault("Solocraft.TrialOfTheCrusaderH", 25.0) },
                {650, sConfigMgr->GetFloatDefault("Solocraft.TrialOfTheChampionH", 5.0) },
                {658, sConfigMgr->GetFloatDefault("Solocraft.PitOfSaronH", 5.0) },
                {668, sConfigMgr->GetFloatDefault("Solocraft.HallsOfReflectionH", 5.0) },
                {724, sConfigMgr->GetFloatDefault("Solocraft.TheRubySanctumH", 25.0) },
                /// CATACLYSM
                {33, sConfigMgr->GetFloatDefault("Solocraft.ShadowfangKeepH", 5.0) }, 
                {36, sConfigMgr->GetFloatDefault("Solocraft.DeadMinesH", 5.0) }, 
                {645, sConfigMgr->GetFloatDefault("Solocraft.BlackrockCavernsH", 5.0) }, 
                {643, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfTheTidesH", 5.0) }, 
                {657, sConfigMgr->GetFloatDefault("Solocraft.TheVortexPinnacleH", 5.0) }, 
                {725, sConfigMgr->GetFloatDefault("Solocraft.TheStonecoreH", 5.0) }, 
                {755, sConfigMgr->GetFloatDefault("Solocraft.LostCityOfTheTol'virH", 5.0) }, 
                {644, sConfigMgr->GetFloatDefault("Solocraft.HallsOfOriginationH", 5.0) }, 
                {670, sConfigMgr->GetFloatDefault("Solocraft.GrimBatolH", 5.0) }, 
                {669, sConfigMgr->GetFloatDefault("Solocraft.BlackwingDescentH", 25.0) }, 
                {671, sConfigMgr->GetFloatDefault("Solocraft.TheBastionOfTwilightH", 25.0) }, 
                {754, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfTheFourWindsH", 25.0) }, 
                {757, sConfigMgr->GetFloatDefault("Solocraft.BaradinHoldH", 25.0) }, 
                {720, sConfigMgr->GetFloatDefault("Solocraft.FirelandsH", 25.0) }, 
                {967, sConfigMgr->GetFloatDefault("Solocraft.DragonSoulH", 25.0) }, 
                {938, sConfigMgr->GetFloatDefault("Solocraft.EndTimeH", 5.0) }, 
                {939, sConfigMgr->GetFloatDefault("Solocraft.WellOfEternityH", 5.0) }, 
                {940, sConfigMgr->GetFloatDefault("Solocraft.HourOfTwilightH", 5.0) }, 
                {859, sConfigMgr->GetFloatDefault("Solocraft.Zul'gurubH", 5.0) }, 
                {568, sConfigMgr->GetFloatDefault("Solocraft.ZulAmanH", 5.0) }, 
                {576, sConfigMgr->GetFloatDefault("Solocraft.NexusH", 5.0) }, 
                /// MISTS OF PANDARIA
                {959, sConfigMgr->GetFloatDefault("Solocraft.ShadoPanMonasteryH", 5.0) }, 
                {1007, sConfigMgr->GetFloatDefault("Solocraft.ScholomanceH", 5.0) }, 
                {1004, sConfigMgr->GetFloatDefault("Solocraft.ScarletMonasteryH", 5.0) }, 
                {994, sConfigMgr->GetFloatDefault("Solocraft.Mogu'shanPalaceH", 5.0) }, 
                {1008, sConfigMgr->GetFloatDefault("Solocraft.Mogu'shanVaultsH", 25.0) }, 
                {1136, sConfigMgr->GetFloatDefault("Solocraft.SiegeOfOrgrimmarH", 25.0) }, 
                {1098, sConfigMgr->GetFloatDefault("Solocraft.ThroneOfThunderH", 25.0) }, 
                {1009, sConfigMgr->GetFloatDefault("Solocraft.HeartOfFearH", 25.0) }, 
                {996, sConfigMgr->GetFloatDefault("Solocraft.TerraceOfEndlessSpringH", 25.0) }, 
                {1001, sConfigMgr->GetFloatDefault("Solocraft.ScarletHallsH", 5.0) }, 
                {962, sConfigMgr->GetFloatDefault("Solocraft.GateOfTheSettingSunH", 5.0) }, 
                {1011, sConfigMgr->GetFloatDefault("Solocraft.SiegeOfNiuzaoTempleH", 5.0) }, 
                {960, sConfigMgr->GetFloatDefault("Solocraft.TempleOfTheJadeSerpentH", 5.0) }, 
                {961, sConfigMgr->GetFloatDefault("Solocraft.StormstoutBreweryH", 5.0) }, 
            };

            D649H10 = sConfigMgr->GetFloatDefault("Solocraft.ArgentTournamentRaidH10", 10.0);  // Trial of the Crusader 10 Heroic
            D649H25 = sConfigMgr->GetFloatDefault("Solocraft.ArgentTournamentRaidH25", 25.0);  // Trial of the Crusader 25 Heroic

        }


    public:
        bool SoloCraftEnable = false;
        bool SoloCraftAnnounceModule = true;
        bool SoloCraftDebuffEnable = true;
        bool SolocraftXPBalancingEnabled = true;
        bool SolocraftXPEnabled = true;
        bool SolocraftNoXPFlag = false; 

        float SoloCraftSpellMult = 1.0;
        float SoloCraftStatsMult = 100.0;
        float SoloCraftXPMod = 1.0; 

        uint32 SolocraftLevelDiff = 1;
        uint32 SolocraftDungeonLevel = 1;   

        std::unordered_map<uint32, uint32> dungeons;
        std::unordered_map<uint32, float> diff_Multiplier;
        std::unordered_map<uint32, float> diff_Multiplier_Heroics;
        std::unordered_map<uint8, uint32> classes;

        float D5 = 1.0;
        float D10 = 1.0;
        float D25 = 1.0;
        float D40 = 1.0;
        float D649H10 = 1.0;
        float D649H25 = 1.0;
};

// Shared XP state so OnGiveXP (announce script) can see who
// currently holds a SoloCraft buff (tracked by instance handler).
// GiveXP returns early when PLAYER_FLAGS_NO_XP_GAIN is set, so
// proportional scaling must NOT set that flag for the balancing case.
static std::unordered_set<uint32> s_solocraftXpScaled;
static std::unordered_map<uint32, bool> s_solocraftHadNoXp;

static uint32 SolocraftGetGuidLow(Player* player)
{
    return player->GetGUID().GetCounter();
}

static void SolocraftRememberNoXpFlag(Player* player)
{
    uint32 guidLow = SolocraftGetGuidLow(player);
    if (s_solocraftHadNoXp.find(guidLow) != s_solocraftHadNoXp.end())
    {
        return;
    }

    s_solocraftHadNoXp[guidLow] = player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
}

static void SolocraftForgetPlayer(uint32 guidLow)
{
    s_solocraftHadNoXp.erase(guidLow);
    s_solocraftXpScaled.erase(guidLow);
}

class solocraft_system_announce : public PlayerScript
{
    public:
        solocraft_system_announce() : PlayerScript("solocraft_system_announce")
        {
        }

        void OnLogin(Player* player) override
        {
            SolocraftConfig const& config = SolocraftConfig::getInstance();
            if (config.SoloCraftEnable && config.SoloCraftAnnounceModule)
            {
                ChatHandler(player->GetSession()).SendSysMessage(SOLOCRAFT_TRINITYSTRING_ACTIVE);
            }
        }

        void OnLogout(Player* player) override
        {
            SolocraftForgetPlayer(SolocraftGetGuidLow(player));
            QueryResult result = CharacterDatabase.PQuery("SELECT `GUID` FROM `custom_solocraft_character_stats` WHERE GUID = {}", player->GetGUID().GetCounter());
            if (result)
            {
                CharacterDatabase.PExecute("DELETE FROM custom_solocraft_character_stats WHERE GUID = {}", player->GetGUID().GetCounter());
            }
        }

        void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) override
        {
            SolocraftConfig const& config = SolocraftConfig::getInstance();
            if (!config.SoloCraftEnable)
            {
                return;
            }

            if (!config.SolocraftXPBalancingEnabled)
            {
                return;
            }

            if (s_solocraftXpScaled.find(SolocraftGetGuidLow(player)) == s_solocraftXpScaled.end())
            {
                return;
            }

            amount = uint32(float(amount) * config.SoloCraftXPMod);
        }
};

class solocraft_system_player_instance_handler : public PlayerScript
{
public:
    solocraft_system_player_instance_handler() : PlayerScript("solocraft_system_player_instance_handler")
    {
    }

    void OnLogout(Player* player) override
    {
        SolocraftForgetPlayer(SolocraftGetGuidLow(player));
    }

    void OnMapChanged(Player* player) override
    {
        if (SolocraftConfig::getInstance().SoloCraftEnable)
        {
            Map* map = player->GetMap();
            float difficulty = CalculateDifficulty(map, player);
            int dunLevel = CalculateDungeonLevel(map, player);
            int numInGroup = GetNumInGroup(player);
            uint32 classBalance = GetClassBalance(player);
            TC_LOG_DEBUG("solocraft", "solocraft player guid = {}, difficulty={}, dunLevel={}, numInGroup={}, classBalance={}", player->GetGUID().GetCounter(), difficulty, dunLevel, numInGroup, classBalance);
            ApplyBuffs(player, map, difficulty, dunLevel, numInGroup, classBalance);
        }
    }

private:
    std::map<uint32, float> _unitDifficulty;

    float CalculateDifficulty(Map* map, Player* /*player*/)
    {
        SolocraftConfig const& config = SolocraftConfig::getInstance();
        if (map)
        {
            if (map->Is25ManRaid())
            {
                if (map->IsHeroic() && map->GetId() == 649)
                {
                    return config.D649H25;
                }
                else if (config.diff_Multiplier_Heroics.find(map->GetId()) == config.diff_Multiplier_Heroics.end())
                {
                    return config.D25;
                }
                else
                    return config.diff_Multiplier_Heroics.at(map->GetId());
            }

            if (map->IsHeroic())
            {
                if (map->GetId() == 649)
                {
                    return config.D649H10;
                }
                else if (config.diff_Multiplier_Heroics.find(map->GetId()) == config.diff_Multiplier_Heroics.end())
                {
                    return config.D10;
                }
                else
                    return config.diff_Multiplier_Heroics.at(map->GetId());
            }

            if (config.diff_Multiplier.find(map->GetId()) == config.diff_Multiplier.end())
            {
                if (map->IsDungeon())
                {
                    return config.D5;
                }
                else if (map->IsRaid())
                {
                    return config.D40;
                }
            }
            else
                return config.diff_Multiplier.at(map->GetId());
        }
        return 0;
    }

    int CalculateDungeonLevel(Map* map, Player* /*player*/)
    {
        SolocraftConfig const& config = SolocraftConfig::getInstance();
        if (config.dungeons.find(map->GetId()) == config.dungeons.end())
        {
            return config.SolocraftDungeonLevel;
        }
        else
            return config.dungeons.at(map->GetId());
    }

    int GetNumInGroup(Player* player)
    {
        int numInGroup = 1;
        Group* group = player->GetGroup();

        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
            numInGroup = groupMembers.size();
        }
        return numInGroup;
    }

    uint32 GetClassBalance(Player* player)
    {
        uint32 classBalance = 100;
        SolocraftConfig const& config = SolocraftConfig::getInstance();

        if (config.classes.find(player->GetClass()) == config.classes.end())
        {
            return classBalance;
        }
        else if (config.classes.at(player->GetClass()) <= 100)
        {
            return config.classes.at(player->GetClass());
        }
        else
            return classBalance;
    }

    void ApplyBuffs(Player* player, Map* map, float difficulty, int dunLevel, int numInGroup, int classBalance)
    {
        SolocraftConfig const& config = SolocraftConfig::getInstance();
        uint32 guidLow = SolocraftGetGuidLow(player);

        if (difficulty <= 0)
        {
            ClearBuffs(player, map);
            return;
        }

        int SpellPowerBonus = 0;

        SolocraftRememberNoXpFlag(player);

        if (player->GetLevel() <= dunLevel + config.SolocraftLevelDiff)
        {
            float GroupDifficulty = GetGroupDifficulty(player);

            if (GroupDifficulty >= difficulty && config.SoloCraftDebuffEnable)
            {
                difficulty = (-abs(difficulty)) + ((((float)classBalance / 100) * difficulty) / numInGroup);
                difficulty = roundf(difficulty * 100) / 100;
                // Proportional XP scaling is handled in OnGiveXP.
                // Do not set PLAYER_FLAGS_NO_XP_GAIN here, otherwise
                // Player::GiveXP returns before the OnGiveXP hook runs.
            }

            QueryResult result = CharacterDatabase.PQuery("SELECT `guid`, `Difficulty`, `GroupSize`, `SpellPower`, `Stats` FROM `custom_solocraft_character_stats` WHERE `guid` = {}", player->GetGUID().GetCounter());

            for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
            {
                if (result)
                {
                    player->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, (*result)[1].GetFloat() * (*result)[4].GetFloat(), false);
                }

                player->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, difficulty * config.SoloCraftStatsMult, true);
            }

            player->SetFullHealth();
            player->CastSpell(player, 6962, true);

            if (player->GetPowerType() == POWER_MANA || player->GetClass() == 11)
            {
                player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

                if (result)
                {
                    player->ApplySpellPowerBonus((*result)[3].GetUInt32() * (*result)[4].GetFloat(), false);
                }

                if (difficulty > 0)
                {
                    SpellPowerBonus = static_cast<int>((player->GetLevel() * config.SoloCraftSpellMult) * difficulty);
                    player->ApplySpellPowerBonus(SpellPowerBonus, true);
                }
            }

            if (!config.SolocraftXPEnabled)
            {
                if (!player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
                {
                    player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
                }
                s_solocraftXpScaled.erase(guidLow);
            }
            else if (config.SolocraftXPBalancingEnabled)
            {
                s_solocraftXpScaled.insert(guidLow);
            }
            else
            {
                s_solocraftXpScaled.erase(guidLow);
            }

            if (difficulty > 0)
            {
                const char* solocraft_enabled = player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_ENABLED);
                const char* solocraft_disabled = player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_DISABLED);
                ChatHandler(player->GetSession()).PSendSysMessage(player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_STATUS), player->GetName().c_str(), map->GetMapName(), difficulty, SpellPowerBonus, classBalance, config.SolocraftXPEnabled ? solocraft_enabled : solocraft_disabled, config.SolocraftXPBalancingEnabled ? solocraft_enabled : solocraft_disabled);
            }
            else
            {
                // Debuffed group case still scales XP proportionally via OnGiveXP.
                s_solocraftXpScaled.insert(guidLow);
                ChatHandler(player->GetSession()).PSendSysMessage(player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_ERR_GROUP_ALREADY_BUFFED), player->GetName().c_str(), map->GetMapName(), difficulty, classBalance);
            }

            CharacterDatabase.PExecute("REPLACE INTO `custom_solocraft_character_stats` (`guid`, `Difficulty`, `GroupSize`, `SpellPower`, `Stats`) VALUES ({}, {}, {}, {}, {})", player->GetGUID().GetCounter(), difficulty, numInGroup, SpellPowerBonus, config.SoloCraftStatsMult);
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage(player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_ERR_LEVEL_TOO_HIGH), player->GetName().c_str(), map->GetMapName(), dunLevel + config.SolocraftLevelDiff);
            ClearBuffs(player, map);
        }
    }

    float GetGroupDifficulty(Player* player)
    {
        float GroupDifficulty = 0.0;
        Group* group = player->GetGroup();

        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();

            for (Group::member_citerator itr = groupMembers.begin(); itr != groupMembers.end(); ++itr)
            {
                if (itr->guid != player->GetGUID())
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT `guid`, `Difficulty`, `GroupSize` FROM `custom_solocraft_character_stats` WHERE `guid` = {}", itr->guid);

                    if (result)
                    {
                        if ((*result)[1].GetFloat() > 0)
                        {
                            GroupDifficulty = GroupDifficulty + (*result)[1].GetFloat();
                        }
                    }
                }
            }
        }

        return GroupDifficulty;
    }

    void ClearBuffs(Player* player, Map* map)
    {
        SolocraftConfig const& config = SolocraftConfig::getInstance();
        uint32 guidLow = SolocraftGetGuidLow(player);
        s_solocraftXpScaled.erase(guidLow);

        QueryResult result = CharacterDatabase.PQuery("SELECT `guid`, `Difficulty`, `GroupSize`, `SpellPower`, `Stats` FROM `custom_solocraft_character_stats` WHERE `guid` = {}", player->GetGUID().GetCounter());

        if (!result)
        {
            SolocraftForgetPlayer(guidLow);
            return;
        }

        float difficulty = (*result)[1].GetFloat();
        int SpellPowerBonus = (*result)[3].GetUInt32();
        float StatsMultPct = (*result)[4].GetFloat();

        ChatHandler(player->GetSession()).PSendSysMessage(player->GetSession()->GetTrinityString(SOLOCRAFT_TRINITYSTRING_CLEAR_BUFFS), player->GetName().c_str(), map->GetMapName(), difficulty, SpellPowerBonus);

        for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
        {
            player->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, difficulty * StatsMultPct, false);
        }

        if (player->GetPowerType() == POWER_MANA && difficulty > 0)
        {
            player->ApplySpellPowerBonus(SpellPowerBonus, false);
        }

        bool hadNoXp = false;
        auto it = s_solocraftHadNoXp.find(guidLow);
        if (it != s_solocraftHadNoXp.end())
        {
            hadNoXp = it->second;
        }

        if (player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN) && !hadNoXp && !config.SolocraftNoXPFlag)
        {
            player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        }

        CharacterDatabase.PExecute("DELETE FROM `custom_solocraft_character_stats` WHERE `guid` = {}", player->GetGUID().GetCounter());
        SolocraftForgetPlayer(guidLow);
    }
};

class solocraft_world_config : public WorldScript
{
public:
    solocraft_world_config() : WorldScript("solocraft_world_config")
    {
    }

    void OnConfigLoad(bool /*reload*/) override
    {
        SolocraftConfig::getInstance().loadConfig();
    }
};

void AddSC_solocraft_system()
{
    new solocraft_world_config();
    new solocraft_system_announce();
    new solocraft_system_player_instance_handler();
}
