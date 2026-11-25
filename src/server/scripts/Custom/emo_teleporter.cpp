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

#include "ScriptPCH.h"

struct struktura
{
	uint32 menu_id, next_menu_id;
	uint8 icon;
	std::string name;
	uint32 cost;
	uint8 level, faction; // 0 Both, 1 Horde, 2 Ally
	uint32 map;
	float x, y, z, o;
};
typedef struct struktura Lokacii;

//	{menu_id, next_menu_id, icon, name, cost, level, faction, map, x, y, z, o}
Lokacii pandariq [] = // key = i & Key
{
    {1,	10,	    3,	"Cities",	0,	0,	0,	0,	0,	0,	0,	0},
    {1,	3,	    3,	"Locations",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	4,	    9,	"Dungeons",	0,	57,	0,	0,	0,	0,	0,	0},
	{1,	5,	    9,	"Raids",	0,	57,	0,	0,	0,	0,	0,	0},
	{1,	6,	    9,	"World Bosses",	0,	57,	0,	0,	0,	0,	0,	0},
	{1,	200,	    9,	"PvP Arenas",	0,	57,	0,	0,	0,	0,	0,	0},
    
    // Locations [MENU]
	{3,	100,	3,	"Eastern Kingdoms",	0,	0,	0,	0,	0,	0,	0,	0},
	{3,	101,	3,	"Kalimdor",	0,	0,	0,	0,	0,	0,	0,	0},
	{3,	102,	3,	"Outland",	0,	58,	0,	0,	0,	0,	0,	0},
	{3,	103,	3,	"Northrend",	0,	68,	0,	0,	0,	0,	0,	0},
	{3,	104,	3,	"Pandaria",	0,	68,	0,	0,	0,	0,	0,	0},
	{3,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
 
    // Dungeons [MENU]
	{4,	11,	9,	"Vanilla Dungeons",	    0,	15,	0,	0,	0,	0,	0,	0},
	{4,	12,	9,	"TBC Dungeons",	        0,	59,	0,	0,	0,	0,	0,	0},
	{4,	13,	9,	"WoTLK Dungeons",	    0,	69,	0,	0,	0,	0,	0,	0},
	{4,	1,	9,	"Cataclysm Dungeons (SOON)",	0,	69,	0,	0,	0,	0,	0,	0}, // trqbva da preprashta vmesto na 1 na 14
	{4,	15,	9,	"Pandaria Dungeons",	0,	69,	0,	0,	0,	0,	0,	0},
	{4,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
 
    // Raids [MENU]
	{5,	16,	3,	"Vanilla Raids",	0,	0,	0,	0,	0,	0,	0,	0},
	{5,	17,	3,	"TBC Raids",	0,	0,	0,	0,	0,	0,	0,	0},
	{5,	18,	3,	"WoTLK Raids",	0,	58,	0,	0,	0,	0,	0,	0},
	{5,	1,	3,	"Cataclysm Raids (SOON)",	0,	68,	0,	0,	0,	0,	0,	0}, // trqbva da preprashta vmesto na 1 na 19
	{5,	20,	3,	"Pandaria Raids",	0,	68,	0,	0,	0,	0,	0,	0},
	{5,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // World Bosses [TELEPORT]
	{6,	0,	2,	"Sha of Anger", 0, 0, 0, 870, 2008.109985f, 1212.219971f, 487.582001f, 0.693056f},
	{6,	0,	2,	"Galleon",	0,	0,	0,	870, -548.515991f, -132.459000f, 156.181000f, 2.025110f},
	{6,	0,	2,	"Nalak, The Storm Lord", 0,	0,	0,	1064, 7046.131348f, 5232.626465f, 84.229630f, 5.451058f},
	{6,	0,	2,	"Oondasta",	0,	0,	0,	870,	5943.290039f,	1071.430054f,	59.731300f,	3.120010f},
	{6,	0,	2,	"Timeless Island - Celestials",	0,	0,	0,	870,	-1650.054016f,	-4891.569824f,	2.056370f,	1.524070f},
	{6,	0,	2,	"Ordos, Fire-God of the Yaungol",	0,	0,	0,	870,	-46.147598f,	-5402.759766f,	150.113998f,	2.911080f},
	{6,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // Cities [TELEPORT]
    {10,	0,	2,	"Shrine of Seven Stars",	0,	0,	2,	870,	865.683167f,	285.115631f,   503.678009f,	   3.702813f},
	{10,	0,	2,	"Shrine of Two Moons",	    0,	0,	1,  870,	1624.060669f,   924.189087f,    471.183533f,    0.130662f},
    {10,	0,	2,	"Stormwind",	0,	0,	2,	0,	-8842.09f,	626.358f,	94.0867f,	3.61363f},
	{10,	0,	2,	"Orgrimmar",	0,	0,	1,	1,	1601.08f,	-4378.69f,	9.9846f,	2.14362f},
	{10,	0,	2,	"Darnassus",	0,	0,	2,	1,	9869.91f,	2493.58f,	1315.88f,	2.78897f},
	{10,	0,	2,	"Ironforge",	0,	0,	2,	0,	-4900.47f,	-962.585f,	501.455f,	5.40538f},
	{10,	0,	2,	"Exodar",	0,	0,	2,	530,	-3864.92f,	-11643.7f,	-137.644f,	5.50862f},
	{10,	0,	2,	"Thunder bluff",	0,	0,	1,	1,	-1274.45f,	71.8601f,	128.159f,	2.80623f},
	{10,	0,	2,	"Undercity",	0,	0,	1,	0,	1633.75f,	240.167f,	-43.1034f,	6.26128f},
	{10,	0,	2,	"Silvermoon city",	0,	0,	1,	530,	9738.28f,	-7454.19f,	13.5605f,	0.043914f},
	{10,	0,	2,	"Dalaran",	0,	67,	0,	571,	5809.55f,	503.975f,	657.526f,	2.38338f},
	{10,	0,	2,	"Shattrath",	0,	57,	0,	530,	-1887.62f,	5359.09f,	-12.4279f,	4.40435f},
	{10,	0,	2,	"Booty bay",	0,	30,	0,	0,	-14281.9f,	552.564f,	8.90422f,	0.860144f},
	{10,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // Dungeons [Vanilla]
	{11,	0,	2,	"Gnomeregan",	0,	25,	2,	0,	-5163.54f,	925.423f,	257.181f,	1.57423f},
	{11,	0,	2,	"The Deadmines",	0,	17,	2,	0,	-11209.6f,	1666.54f,	24.6974f,	1.42053f},
	{11,	0,	2,	"The Stockade",	0,	22,	2,	0,	-8799.15f,	832.718f,	97.6348f,	6.04085f},
	{11,	0,	2,	"Ragefire Chasm",	0,	15,	1,	1,	1811.78f,	-4410.5f,	-18.4704f,	5.20165f},
	{11,	0,	2,	"Razorfen Downs",	0,	34,	1,	1,	-4657.3f,	-2519.35f,	81.0529f,	4.54808f},
	{11,	0,	2,	"Razorfen Kraul",	0,	24,	1,	1,	-4470.28f,	-1677.77f,	81.3925f,	1.16302f},
	{11,	0,	2,	"Scarlet Monastery",	0,	32,	1,	0,	2873.15f,	-764.523f,	160.332f,	5.10447f},
	{11,	0,	2,	"Shadowfang Keep",	0,	18,	1,	0,	-234.675f,	1561.63f,	76.8921f,	1.24031f},
	{11,	0,	2,	"Wailing Caverns",	0,	17,	1,	1,	-731.607f,	-2218.39f,	17.0281f,	2.78486f},
	{11,	0,	2,	"Blackfathom Deeps",	0,	21,	0,	1,	4249.99f,	740.102f,	-25.671f,	1.34062f},
	{11,	0,	2,	"Blackrock Depths",	0,	53,	0,	0,	-7179.34f,	-921.212f,	165.821f,	5.09599f},
	{11,	0,	2,	"Blackrock Spire",	0,	57,	0,	0,	-7527.05f,	-1226.77f,	285.732f,	5.29626f},
	{11,	0,	2,	"Dire Maul",	0,	55,	0,	1,	-3520.14f,	1119.38f,	161.025f,	4.70454f},
	{11,	0,	2,	"Maraudon",	0,	45,	0,	1,	-1421.42f,	2907.83f,	137.415f,	1.70718f},
	{11,	0,	2,	"Scholomance",	0,	55,	0,	0,	1269.64f,	-2556.21f,	93.6088f,	0.620623f},
	{11,	0,	2,	"Stratholme",	0,	55,	0,	0,	3352.92f,	-3379.03f,	144.782f,	6.25978f},
	{11,	0,	2,	"Sunken Temple",	0,	47,	0,	0,	-10177.9f,	-3994.9f,	-111.239f,	6.01885f},
	{11,	0,	2,	"Uldaman",	0,	37,	0,	0,	-6071.37f,	-2955.16f,	209.782f,	0.015708f},
	{11,	0,	2,	"Zul'Farrak",	0,	35,	0,	1,	-6801.19f,	-2893.02f,	9.00388f,	0.158639f},
	{11,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Dungeons [TBC]
	{12,	0,	2,	"Auchindoun",	0,	64,	0,	530,	-3324.49f,	4943.45f,	-101.239f,	4.63901f},
	{12,	0,	2,	"Caverns of Time",	0,	66,	0,	1,	-8369.65f,	-4253.11f,	-204.272f,	-2.70526f},
	{12,	0,	2,	"Coilfang Reservoir",	0,	62,	0,	530,	738.865f,	6865.77f,	-69.4659f,	6.27655f},
	{12,	0,	2,	"Hellfire Citadel",	0,	59,	0,	530,	-347.29f,	3089.82f,	21.394f,	5.68114f},
	{12,	0,	2,	"Magisters' Terrace",	0,	70,	0,	530,	12884.6f,	-7317.69f,	65.5023f,	4.799f},
	{12,	0,	2,	"Tempest Keep",	0,	70,	0,	530,	3100.48f,	1536.49f,	190.3f,	4.62226f},
	{12,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Dungeons [WoTLK]
	{13,	0,	2,	"Azjol-Nerub",	0,	73,	0,	571,	3707.86f,	2150.23f,	36.76f,	3.22f},
	{13,	0,	2,	"The Culling of Stratholme",	0,	79,	0,	1,	-8756.39f,	-4440.68f,	-199.489f,	4.66289f},
	{13,	0,	2,	"Trial of the Champion",	0,	79,	0,	571,	8590.95f,	791.792f,	558.235f,	3.13127f},
	{13,	0,	2,	"Drak'Tharon Keep",	0,	74,	0,	571,	4765.59f,	-2038.24f,	229.363f,	0.887627f},
	{13,	0,	2,	"Gundrak",	0,	71,	0,	571,	6722.44f,	-4640.67f,	450.632f,	3.91123f},
	{13,	0,	2,	"Icecrown Citadel Dungeons",	0,	79,	0,	571,	5643.16f,	2028.81f,	798.274f,	4.60242f},
	{13,	0,	2,	"The Nexus Dungeons",	0,	71,	0,	571,	3782.89f,	6965.23f,	105.088f,	6.14194f},
	{13,	0,	2,	"The Violet Hold",	0,	75,	0,	571,	5693.08f,	502.588f,	652.672f,	4.0229f},
	{13,	0,	2,	"Halls of Lightning",	0,	79,	0,	571,	9136.52f,	-1311.81f,	1066.29f,	5.19113f},
	{13,	0,	2,	"Halls of Stone",	0,	77,	0,	571,	8922.12f,	-1009.16f,	1039.56f,	1.57044f},
	{13,	0,	2,	"Utgarde Keep",	0,	69,	0,	571,	1203.41f,	-4868.59f,	41.2486f,	0.283237f},
	{13,	0,	2,	"Utgarde Pinnacle",	0,	75,	0,	571,	1267.24f,	-4857.3f,	215.764f,	3.22768f},
	{13,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Cataclysm Dungeons [TELEPORT]
	/*{14,	0,	2,	"Blackrock Caverns",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Deadmines",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Grim Batol",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Halls of Origination",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Lost City of the TolVir",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Shadowfang Keep",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Throne of Tides",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"The Vortex Pinnacle",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"The Stonecore",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Zul Aman",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Zul Gurub",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Well of Eternity",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"Hour of Twilight",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	0,	2,	"End Time",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{14,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},*/
    // Pandaria Dungeons [TELEPORT]
	{15,	0,	2,	"Gate of the Setting Sun", 0, 0, 0, 870, 690.999084f, 2079.934570f, 374.781830f, 0.50658f},
	{15,	0,	2,	"Mogushan Palace",	0, 0, 0, 870, 1390.092651f, 439.846649f, 478.981323f, 5.411988f},
	{15,	0,	2,	"Shado-Pan Monastery", 0, 0, 0, 870, 3634.507966f, 2540.199951f, 769.950745f, 0.423151f},
	{15,	0,	2,	"Siege of Niuzao Temple", 0, 0,	0, 870, 1438.815430f, 5087.358398f,	137.413757f, 0.525264f},
	{15,	0,	2,	"Stormstout Brewery",	0,	0,	0,	870, -711.619202f,	1263.815918f, 136.023697f, 3.444620f},
	{15,	0,	2,	"Temple of the Jade Serpent",	0,	0,	0,	870, 959.246765f, -2470.858154f, 180.509155f, 4.371413f},
	{15,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // Raids [Teleport]
    // vanilla
	{16,	0,	2,	"Blackwing Lair",	0,	60,	0,	229,	152.451f,	-474.881f,	116.84f,	0.001073f},
	{16,	0,	2,	"Zul'Gurub",	0,	57,	0,	0,	-11916.7f,	-1215.72f,	92.289f,	4.72454f},
	{16,	0,	2,	"Molten Core",	0,	60,	0,	230,	1126.64f,	-459.94f,	-102.535f,	3.46095f},
	{16,	0,	2,	"Temple of Ahn'Qiraj",	0,	60,	0,	1,	-8240.09f,	1991.32f,	129.072f,	0.941603f},
	{16,	0,	2,	"Ruins of Ahn'Qiraj",	0,	60,	0,	1,	-8409.82f,	1499.06f,	27.7179f,	2.51868f},
    // tbc
	{17,	0,	2,	"Black Temple",	0,	70,	0,	530,	-3649.92f, 317.469f, 35.2827f, 2.94285f},
	{17,	0,	2,	"Zul'Aman",	0,	70,	1,	530,	6851.78f,	-7972.57f,	179.242f, 4.64691f},
	{17,	0,	2,	"Hyjal Summit",	0,	70,	0,	1,	-8177.89f,	-4181.23f,	-167.552f,	0.913338f},
	{17,	0,	2,	"Serpentshrine Cavern",	0,	70,	0,	530,	797.855f,	6865.77f,	-65.4165f,	0.005938f},
	{17,	0,	2,	"Gruul's Lair",	0,	70,	0,	530,	3530.06f,	5104.08f,	3.50861f,	5.51117f},
	{17,	0,	2,	"Magtheridon's Lair",	0,	70,	0,	530,	-336.411f,	3130.46f,	-102.928f,	5.20322f},
	{17,	0,	2,	"Karazhan",	0,	70,	0,	0,	-11118.9f,	-2010.33f,	47.0819f,	0.649895f},
	{17,	0,	2,	"Sunwell Plateau",	0,	70,	0,	530,	12574.1f,	-6774.81f,	15.0904f,	3.13788f},
    // wotlk
	{18,	0,	2,	"Naxxramas",	0,	80,	0,	571,	3668.72f,	-1262.46f,	243.622f,	4.785f},
	{18,	0,	2,	"Onyxia's Lair",	0,	80,	0,	1,	-4708.27f,	-3727.64f,	54.5589f,	3.72786f},
	{18,	0,	2,	"The Eye",	0,	67,	0,	530,	3088.49f,	1381.57f,	184.863f,	4.61973f},
	{18,	0,	2,	"The Eye of Eternity",	0,	80,	0,	571,	3784.17f,	7028.84f,	161.258f,	5.79993f},
	{18,	0,	2,	"The Obsidian Sanctum",	0,	80,	0,	571,	3472.43f,	264.923f,	-120.146f,	3.27923f},
	{18,	0,	2,	"Vault of Archavon",	0,	80,	0,	571,	5453.72f,	2840.79f,	421.28f,	0.0f},
	{18,	0,	2,	"Ulduar",	0,	80,	0,	571,	9222.88f,	-1113.59f,	1216.12f,	6.27549f},
	{18,	0,	2,	"Trial of the Crusader",	0,	80,	0,	571,	8515.61f,	714.153f,	558.248f,	1.57753f},
	{18,	0,	2,	"Icecrown Citadel",	0,	80,	0,	571,	5855.22f,	2102.03f,	635.991f,	3.57899f},
	{18,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // cataclysm
	/*{19,	0,	2,	"Baradin Hold",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	0,	2,	"Blackwing Descent",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	0,	2,	"The Bastion of Twilight",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	0,	2,	"Throne of the Four Winds",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	0,	2,	"Firelands",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	0,	2,	"Dragon Soul",	0,	0,	0,	map,	xf,	yf,	zf,	of},
	{19,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},*/
    // pandaria
	{20,	0,	2,	"Heart of Fear",	0,	0,	0,	870,	167.249481f,	4058.351074f,	255.908676f, 1.746353f},
	{20,	0,	2,	"Mogushan Vaults",	0,	0,	0,	870,	3982.903564f,	1110.826294f,	497.139557f,	2.180673f},
	{20,	0,	2,	"Terrace of Endless Spring",	0,	0,	0,	870,	956.426880f,	-54.437237f,	512.490295f,	0.917750f},
	{20,	0,	2,	"Throne of Thunder",	0,	0,	0,	1064,	6888.330566f,	5518.514160f,	1.851241f,	5.583782f},
	{20,	0,	2,	"Siege of Orgrimmar",	0,	0,	0,	870,	1236.162598f,	607.475769f,	318.770935f,	5.503720f},
	{20,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // Locations [Eastern Kingdooms]
	{100,	0,	2,	"Elwynn Forest",	0,	0,	2,	0,	-9449.06f,	64.8392f,	56.3581f,	3.07047f},
	{100,	0,	2,	"Eversong Woods",	0,	0,	1,	530,	9024.37f,	-6682.55f,	16.8973f,	3.14131f},
	{100,	0,	2,	"Dun Morogh",	0,	0,	2,	0,	-5603.76f,	-482.704f,	396.98f,	5.23499f},
	{100,	0,	2,	"Tirisfal Glades",	0,	0,	1,	0,	2274.95f,	323.918f,	34.1137f,	4.24367f},
	{100,	0,	2,	"Ghostlands",	0,	10,	1,	530,	7595.73f,	-6819.6f,	84.3718f,	2.56561f},
	{100,	0,	2,	"Loch modan",	0,	10,	2,	0,	-5405.85f,	-2894.15f,	341.972f,	5.48238f},
	{100,	0,	2,	"Silverpine Forest",	0,	10,	1,	0,	505.126f,	1504.63f,	124.808f,	1.77987f},
	{100,	0,	2,	"Westfall",	0,	10,	2,	0,	-10684.9f,	1033.63f,	32.5389f,	6.07384f},
	{100,	0,	2,	"Redridge mountains",	0,	15,	2,	0,	-9447.8f,	-2270.85f,	71.8224f,	0.283853f},
	{100,	0,	2,	"Duskwood",	0,	18,	2,	0,	-10531.7f,	-1281.91f,	38.8647f,	1.56959f},
	{100,	0,	2,	"Hillsbrad Foothills",	0,	20,	0,	0,	-385.805f,	-787.954f,	54.6655f,	1.03926f},
	{100,	0,	2,	"Wetlands",	0,	20,	2,	0,	-3517.75f,	-913.401f,	8.86625f,	2.60705f},
	{100,	0,	2,	"Alterac Mountains",	0,	30,	0,	0,	275.049f,	-652.044f,	130.296f,	0.502032f},
	{100,	0,	2,	"Arathi Highlands",	0,	30,	0,	0,	-1581.45f,	-2704.06f,	35.4168f,	0.490373f},
	{100,	0,	2,	"Stranglethorn Vale",	0,	30,	0,	0,	-11921.7f,	-59.544f,	39.7262f,	3.73574f},
	{100,	0,	2,	"Badlands",	0,	35,	0,	0,	-6782.56f,	-3128.14f,	240.48f,	5.65912f},
	{100,	0,	2,	"Swamp of Sorrows",	0,	35,	0,	0,	-10368.6f,	-2731.3f,	21.6537f,	5.29238f},
	{100,	0,	2,	"The Hinterlands",	0,	40,	0,	0,	112.406f,	-3929.74f,	136.358f,	0.981903f},
	{100,	0,	2,	"Searing Gorge",	0,	43,	0,	0,	-6686.33f,	-1198.55f,	240.027f,	0.916887f},
	{100,	0,	2,	"The Blasted Lands",	0,	45,	0,	0,	-11184.7f,	-3019.31f,	7.29238f,	3.20542f},
	{100,	0,	2,	"Burning Steppes",	0,	50,	0,	0,	-7979.78f,	-2105.72f,	127.919f,	5.10148f},
	{100,	0,	2,	"Western Plaguelands",	0,	51,	0,	0,	1743.69f,	-1723.86f,	59.6648f,	5.23722f},
	{100,	0,	2,	"Eastern Plaguelands",	0,	53,	0,	0,	2280.64f,	-5275.05f,	82.0166f,	4.7479f},
	{100,	0,	2,	"Isle of Quel'Danas",	0,	70,	0,	530,	12806.5f,	-6911.11f,	41.1156f,	2.22935f},
	{100,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Locations [Kalimdor]
	{101,	0,	2,	"Azuremyst Isle",	0,	0,	2,	530,	-4192.62f,	-12576.7f,	36.7598f,	1.62813f},
	{101,	0,	2,	"Teldrassil",	0,	0,	2,	1,	9889.03f,	915.869f,	1307.43f,	1.9336f},
	{101,	0,	2,	"Durotar",	0,	0,	1,	1,	228.978f,	-4741.87f,	10.1027f,	0.416883f},
	{101,	0,	2,	"Mulgore",	0,	0,	1,	1,	-2473.87f,	-501.225f,	-9.42465f,	0.6525f},
	{101,	0,	2,	"Bloodmyst Isle",	0,	15,	2,	530,	-2095.7f,	-11841.1f,	51.1557f,	6.19288f},
	{101,	0,	2,	"Darkshore",	0,	10,	2,	1,	6463.25f,	683.986f,	8.92792f,	4.33534f},
	{101,	0,	2,	"The Barrens",	0,	10,	1,	1,	-575.772f,	-2652.45f,	95.6384f,	0.006469f},
	{101,	0,	2,	"Stonetalon Mountains",	0,	15,	0,	1,	1574.89f,	1031.57f,	137.442f,	3.8013f},
	{101,	0,	2,	"Ashenvale Forest",	0,	18,	0,	1,	1919.77f,	-2169.68f,	94.6729f,	6.14177f},
	{101,	0,	2,	"Thousand Needles",	0,	25,	1,	1,	-5375.53f,	-2509.2f,	-40.432f,	2.41885f},
	{101,	0,	2,	"Desolace",	0,	30,	0,	1,	-656.056f,	1510.12f,	88.3746f,	3.29553f},
	{101,	0,	2,	"Dustwallow Marsh",	0,	35,	0,	1,	-3350.12f,	-3064.85f,	33.0364f,	5.12666f},
	{101,	0,	2,	"Feralas",	0,	40,	0,	1,	-4808.31f,	1040.51f,	103.769f,	2.90655f},
	{101,	0,	2,	"Tanaris Desert",	0,	40,	0,	1,	-6940.91f,	-3725.7f,	48.9381f,	3.11174f},
	{101,	0,	2,	"Azshara",	0,	45,	0,	1,	3117.12f,	-4387.97f,	91.9059f,	5.49897f},
	{101,	0,	2,	"Felwood",	0,	48,	0,	1,	3898.8f,	-1283.33f,	220.519f,	6.24307f},
	{101,	0,	2,	"Un'Goro Crater",	0,	48,	0,	1,	-6291.55f,	-1158.62f,	-258.138f,	0.457099f},
	{101,	0,	2,	"Silithus",	0,	55,	0,	1,	-6815.25f,	730.015f,	40.9483f,	2.39066f},
	{101,	0,	2,	"Winterspring",	0,	55,	0,	1,	6658.57f,	-4553.48f,	718.019f,	5.18088f},
	{101,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Locations [Outland]
	{102,	0,	2,	"Hellfire Peninsula",	0,	58,	0,	530,	-207.335f,	2035.92f,	96.464f,	1.59676f},
	{102,	0,	2,	"Zangarmarsh",	0,	60,	0,	530,	-220.297f,	5378.58f,	23.3223f,	1.61718f},
	{102,	0,	2,	"Terokkar Forest",	0,	62,	0,	530,	-2266.23f,	4244.73f,	1.47728f,	3.68426f},
	{102,	0,	2,	"Nagrand",	0,	64,	0,	530,	-1610.85f,	7733.62f,	-17.2773f,	1.33522f},
	{102,	0,	2,	"Blade's Edge Mountains",	0,	65,	0,	530,	2029.75f,	6232.07f,	133.495f,	1.30395f},
	{102,	0,	2,	"Netherstorm",	0,	67,	0,	530,	3271.2f,	3811.61f,	143.153f,	3.44101f},
	{102,	0,	2,	"Shadowmoon Valley",	0,	67,	0,	530,	-3681.01f,	2350.76f,	76.587f,	4.25995f},
	{102,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Locations [Northrend]
	{103,	0,	2,	"Kun-Lai Summit",	0,	68,	0,	571,	2954.24f,	5379.13f,	60.4538f,	2.55544f},
	{103,	0,	2,	"Howling Fjord",	0,	68,	0,	571,	682.848f,	-3978.3f,	230.161f,	1.54207f},
	{103,	0,	2,	"Dragonblight",	0,	71,	0,	571,	2678.17f,	891.826f,	4.37494f,	0.101121f},
	{103,	0,	2,	"Grizzly Hills",	0,	73,	0,	571,	4017.35f,	-3403.85f,	290.0f,	5.35431f},
	{103,	0,	2,	"Zul'Drak",	0,	74,	0,	571,	5560.23f,	-3211.66f,	371.709f,	5.55055f},
	{103,	0,	2,	"Sholazar Basin",	0,	76,	0,	571,	5614.67f,	5818.86f,	-69.722f,	3.60807f},
	{103,	0,	2,	"Crystalsong Forest",	0,	77,	0,	571,	5411.17f,	-966.37f,	167.082f,	1.57167f},
	{103,	0,	2,	"Timeless Isle",	0,	77,	0,	571,	6120.46f,	-1013.89f,	408.39f,	5.12322f},
	{103,	0,	2,	"Isle of Thunder",	0,	77,	0,	571,	8323.28f,	2763.5f,	655.093f,	2.87223f},
	{103,	0,	2,	"Isle of Giants",	0,	77,	0,	571,	4522.23f,	2828.01f,	389.975f,	0.215009f},
	{103,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
    // Locations [Pandaria]
	{104,	0,	2,	"The Jade Forest",	0,	0,	0,	870,	1495.146729f, -1813.068115f , 246.177261f , 2.647196f},
	{104,	0,	2,	"Kun-Lai Summit",	0,	0,	0,	870,	3085.287598f , 1216.399414f , 653.639221f , 3.633658f},
	{104,	0,	2,	"Townlong Steppes",	0,	0,	0,	870,	1832.156616f , 4280.191895f , 149.524429f , 2.070713f},
	{104,	0,	2,	"Valley of Eternal Blossoms",	0,	0,	0,	870,	1208.697998f , 1377.029297f , 363.664032f , 2.333821f},
	{104,	0,	2,	"Dread Wastes",	0,	0,	0,	870,	130.026199f , 3199.298340f , 214.760498f , 0.036534f},
	{104,	0,	2,	"Valley of the Four Winds",	0,	0,	0,	870,	-204.379898f , 495.047516f , 187.190201f , 2.644060f},
	{104,	0,	2,	"Karasang Wilds",	0,	0,	0,	870,	-1169.963013f , 1020.275940f , 21.745750f , 3.943096f},
	{104,	0,	2,	"Isle of Thunder",	0,	0,	0,	1064, 7224.341797f, 6282.499512f, 12.471354f, 2.368384f},
	{104,	0,	2,	"Isle of Giants",	0,	0,	0,	870,	5802.028320f , 1085.182373f , 0.869473f , 0.268230f},
	{104,	0,	2,	"Timeless Isle",	0,	0,	0,	870,	-649.949829f , -4877.501465f , 2.057448f , 4.724567f},
	{104,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

    // PVP Arenas [TELEPORT]
	{200,	0,	2,	"Gurubashi Arena",	0,	30,	0,	0,	-13181.8f,	339.356f,	42.9805f,	1.18013f},
	{200,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
};

// TextIDs from npc_text
enum eEnums
{
	TEXT_MAIN_H		=	300000,			//Horde main menu text
	TEXT_MAIN_A		=	300001,			//Alliance main menu text
	TEXT_DUNGEON	=	300002,			//Dungeon teleport menu texts
	TEXT_RAID		=	300003,			//Raid teleport menu text
	TEXT_AREA		=	300004,			//Area teleport location menu texts
	
	TELEPORT_COUNT	=	sizeof pandariq/sizeof(*pandariq),
};

#define ARE_YOU_SURE	"Are you sure, that you want to go to "
#define ERROR_COMBAT	"|cffff0000You are in combat|r"

bool Custom_FactCheck (uint32 Fact, unsigned char Key)
{
	bool Show = false;
	switch (pandariq[Key].faction)
	{
	case 0:
		Show = true;
		break;
	case 1:
		if (Fact == HORDE)
			Show = true;
		break;
	case 2:
		if (Fact == ALLIANCE)
			Show = true;
		break;
	}
	return (Show);
}

uint32 Custom_GetText (unsigned int menu, Player* pPlayer)
{
	uint32 TEXT = TEXT_AREA;
	switch (menu)
	{
	case 0:
		switch (pPlayer->GetTeam())
		{
		case ALLIANCE:
			TEXT = TEXT_MAIN_A;
			break;
		case HORDE:
			TEXT = TEXT_MAIN_H;
			break;
		}
	case 1:
	case 2:
	case 3:
		TEXT = TEXT_DUNGEON;
		break;
	case 4:
		TEXT = TEXT_RAID;
		break;
	}
	return (TEXT);
}

void Custom_GetMenu (Player* pPlayer, Creature* pCreature, uint32 Key)
{
	bool ENDMENU = false;
	for(uint32 i = 0; i < TELEPORT_COUNT; i++)
	{
		if (ENDMENU && pandariq[i].menu_id != Key)
			break;
		if (pandariq[i].menu_id == Key && pPlayer->GetLevel() >= pandariq[i].level && Custom_FactCheck(pPlayer->GetTeam(), i))
		{
			if (pandariq[i].next_menu_id != 0)
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(pandariq[i].icon, pandariq[i].name, GOSSIP_SENDER_MAIN, i, "", pandariq[i].cost, false);
			else
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(pandariq[i].icon, pandariq[i].name, GOSSIP_SENDER_MAIN, i, ARE_YOU_SURE+pandariq[i].name, pandariq[i].cost, false);
			ENDMENU = true;
		}
	}
	pPlayer->PlayerTalkClass->SendGossipMenu(Custom_GetText(Key, pPlayer), pCreature->GetGUID());
}

class npc_world_teleporter : public CreatureScript
{
	public:
	npc_world_teleporter(): CreatureScript("npc_world_teleporter") { }






        struct npc_world_teleporterAI : public ScriptedAI
        {
            npc_world_teleporterAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipHello(Player* player) override
            {

            Custom_GetMenu(player, me, 1);
            return true;
	
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

            player->PlayerTalkClass->ClearMenus(); // clear the menu

            player->ModifyMoney(-1 * pandariq[action].cost); // take cash
            uint32 Key = pandariq[action].next_menu_id;
            if (Key == 0) // if no next_menu_id, then teleport to coords
            {
            if (!player->IsInCombat())
            {
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(pandariq[action].map, pandariq[action].x, pandariq[action].y, pandariq[action].z, pandariq[action].o);
            return true;
            }
            player->GetSession()->SendAreaTriggerMessage(ERROR_COMBAT);
            Key = pandariq[action].menu_id;
            }

            Custom_GetMenu(player, me, Key);
            return true;
	
            }
};

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_world_teleporterAI(creature);
        }
};

void AddSC_npc_world_teleporter()
{
    new npc_world_teleporter();
}
