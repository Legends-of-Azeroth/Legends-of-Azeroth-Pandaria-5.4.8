/*
 * Copyright (C) 2011-2016 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2016 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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

#ifndef _MMAP_TERRAIN_BUILDER_H
#define _MMAP_TERRAIN_BUILDER_H

#include "PathCommon.h"
#include "WorldModel.h"

#include <map>
#include "G3D/Array.h"
#include "G3D/Vector3.h"
#include "G3D/Matrix3.h"

namespace MMAP
{
        //               MapID           DisplayID  ModelName
    static std::map<uint32, std::pair<uint32, char const*>> const transportMaps =
    {
        { 582, { 7087, "Transportship_NE.wmo"               } }, // Ship, Night Elf (Moonspray)
        { 584, { 3015, "transportship.wmo"                  } }, // Ship (The Lady Mehley)
        { 586, { 7087, "Transportship_NE.wmo"               } }, // Ship, Night Elf (Elune's Blessing)
        { 587, { 7087, "Transportship_NE.wmo"               } }, // Ship, Night Elf (Feathermoon Ferry)
        { 588, { 3015, "transportship.wmo"                  } }, // Ship (The Bravery)
        { 589, { 3031, "transport_zeppelin.wmo"             } }, // Zeppelin (The Iron Eagle)
        { 590, { 3031, "transport_zeppelin.wmo"             } }, // Zeppelin (The Purple Princess)
        { 591, { 3031, "transport_zeppelin.wmo"             } }, // Zeppelin (The Thundercaller)
        { 592, {    0, ""                                   } },
        { 593, { 3015, "transportship.wmo"                  } }, // Ship (The Maiden's Fancy)
        { 594, { 7570, "Transport_Pirate_ship.wmo"          } }, // Sister Mercy
        { 596, {    0, ""                                   } },
        { 610, { 7546, "Transport_Horde_Zeppelin.wmo"       } }, // Zeppelin, Horde (Cloudkisser)
        { 612, { 7446, "Transport_Icebreaker_ship.wmo"      } }, // Ship, Icebreaker (Northspear)
        { 613, { 7546, "Transport_Horde_Zeppelin.wmo"       } }, // Zeppelin, Horde (The Mighty Wind)
        { 614, { 7446, "Transport_Icebreaker_ship.wmo"      } }, // Ship, Icebreaker (Stormwind's Pride)
        { 620, { 7636, "Transport_Tuskarr_ship.wmo"         } }, // Turtle (Walker of Waves)
        { 621, { 7636, "Transport_Tuskarr_ship.wmo"         } }, // Turtle (Green Island)
        { 622, { 8253, "ND_HordeGunship.wmo"                } }, // Orgrim's Hammer
        { 623, { 8254, "ND_AllianceGunship.wmo"             } }, // The Skybreaker
        { 641, { 9002, "nd_alliancegunship_bg.wmo"          } }, // Alliance Gunship
        { 642, { 9001, "nd_hordegunship_bg.wmo"             } }, // Horde Gunship
        { 647, { 3031, "transport_zeppelin.wmo"             } }, // The Zephyr
        { 672, { 9150, "nd_alliancegunship_icecrown.wmo"    } }, // The Skybreaker
        { 673, { 9151, "nd_hordegunship_icecrown.wmo"       } }, // Orgrim's Hammer
        { 712, { 9002, "nd_alliancegunship_bg.wmo"          } }, // The Skybreaker
        { 713, { 8253, "ND_HordeGunship.wmo"                } }, // Orgrim's Hammer
        { 718, { 7546, "Transport_Horde_Zeppelin.wmo"       } }, // Zeppelin, Horde (The Mighty Wind) (Icecrown Raid)
        { 738, { 9534, "transport_pirate_ship02.wmo"        } }, // Ship to Vashj'ir
        { 739, { 9583, "horde_submarine.wmo"                } }, // Horde Submarine to Leviathan Cave
        { 740, { 9582, "alliance_submarine.wmo"             } }, // Alliance Submarine to Leviathan Cave
        { 741, { 7546, "Transport_Horde_Zeppelin.wmo"       } }, // The Spear of Durotar
        { 742, { 10308,"horde_submarine_closed.wmo"         } }, // Horde Submarine circling Abyssal Maw
        { 743, { 10404,"ALLIANCE_SUBMARINE_SEALED.WMO"      } }, // Alliance Submarine circling Abyssal Maw
        { 747, { 8254, "ND_AllianceGunship.wmo"             } }, // Alliance Gunship
        { 749, { 8253, "ND_HordeGunship.wmo"                } }, // Orc Gunship
        { 762, { 3031, "transport_zeppelin.wmo"             } }, // Krazzworks to Dragonmaw Port
        { 763, { 3031, "transport_zeppelin.wmo"             } }, // Dragonmaw Port to Krazzworks
        { 765, { 3031, "transport_zeppelin.wmo"             } }, // Krazzworks Attack Zeppelin
        { 1133,{ 13656,""                                   } }, // Zandalari Ship (wmo not exist?)
    };

    enum Spot
    {
        TOP     = 1,
        RIGHT   = 2,
        LEFT    = 3,
        BOTTOM  = 4,
        ENTIRE  = 5
    };

    enum Grid
    {
        GRID_V8,
        GRID_V9
    };

    static const int V9_SIZE = 129;
    static const int V9_SIZE_SQ = V9_SIZE*V9_SIZE;
    static const int V8_SIZE = 128;
    static const int V8_SIZE_SQ = V8_SIZE*V8_SIZE;
    static const float GRID_SIZE = 533.3333f;
    static const float GRID_PART_SIZE = GRID_SIZE/V8_SIZE;

    // see contrib/extractor/system.cpp, CONF_use_minHeight
    static const float INVALID_MAP_LIQ_HEIGHT = -500.f;
    static const float INVALID_MAP_LIQ_HEIGHT_MAX = 5000.0f;

    // see following files:
    // contrib/extractor/system.cpp
    // src/game/Map.cpp

    struct MeshData
    {
        G3D::Array<float> solidVerts;
        G3D::Array<int> solidTris;

        G3D::Array<float> liquidVerts;
        G3D::Array<int> liquidTris;
        G3D::Array<uint8> liquidType;

        // offmesh connection data
        G3D::Array<float> offMeshConnections;   // [p0y,p0z,p0x,p1y,p1z,p1x] - per connection
        G3D::Array<float> offMeshConnectionRads;
        G3D::Array<unsigned char> offMeshConnectionDirs;
        G3D::Array<unsigned char> offMeshConnectionsAreas;
        G3D::Array<unsigned short> offMeshConnectionsFlags;
    };

    class TerrainBuilder
    {
        public:
            TerrainBuilder(bool skipLiquid);
            ~TerrainBuilder();

            TerrainBuilder(TerrainBuilder const& tb) = delete;

            void loadMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData);
            bool loadVMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData);
            void loadOffMeshConnections(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData, const char* offMeshFilePath);

            bool usesLiquids() { return !m_skipLiquid; }

            // vert and triangle methods
            static void transform(std::vector<G3D::Vector3> &original, std::vector<G3D::Vector3> &transformed,
                float scale, G3D::Matrix3 &rotation, G3D::Vector3 &position);
            static void copyVertices(std::vector<G3D::Vector3> &source, G3D::Array<float> &dest);
            static void copyIndices(std::vector<VMAP::MeshTriangle> &source, G3D::Array<int> &dest, int offest, bool flip);
            static void copyIndices(G3D::Array<int> &src, G3D::Array<int> &dest, int offset);
            static void cleanVertices(G3D::Array<float> &verts, G3D::Array<int> &tris);
        private:
            /// Loads a portion of a map's terrain
            bool loadMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData, Spot portion);

            /// Sets loop variables for selecting only certain parts of a map's terrain
            void getLoopVars(Spot portion, int &loopStart, int &loopEnd, int &loopInc);

            /// Controls whether liquids are loaded
            bool m_skipLiquid;

            /// Get the vector coordinate for a specific position
            void getHeightCoord(int index, Grid grid, float xOffset, float yOffset, float* coord, float* v);

            /// Get the triangle's vector indices for a specific position
            void getHeightTriangle(int square, Spot triangle, int* indices, bool liquid = false);

            /// Determines if the specific position's triangles should be rendered
            bool isHole(int square, const uint16 holes[16][16]);

            /// Get the liquid vector coordinate for a specific position
            void getLiquidCoord(int index, int index2, float xOffset, float yOffset, float* coord, float* v);

            /// Get the liquid type for a specific position
            uint8 getLiquidType(int square, const uint8 liquid_type[16][16]);
    };
}

#endif

