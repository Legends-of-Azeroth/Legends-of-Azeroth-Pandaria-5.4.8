# This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Enable precompiled headers when using the GCC compiler.

if (USE_SCRIPTPCH)
  set(scripts_STAT_PCH_HDR PrecompiledHeaders/ScriptPCH.h)
endif ()

file(GLOB_RECURSE sources_PrecompiledHeaders PrecompiledHeaders/*.cpp PrecompiledHeaders/*.h)
file(GLOB_RECURSE sources_Battlegrounds Battlegrounds/*.cpp  Battlegrounds/*.h)
file(GLOB_RECURSE sources_EasternKingdoms EasternKingdoms/*.cpp EasternKingdoms/*.h)
file(GLOB_RECURSE sources_Kalimdor Kalimdor/*.cpp Kalimdor/*.h)
file(GLOB_RECURSE sources_Outland Outland/*.cpp Outland/*.h)
file(GLOB_RECURSE sources_Northrend Northrend/*.cpp Northrend/*.h)
file(GLOB_RECURSE sources_Maelstrom Maelstrom/*.cpp Maelstrom/*.h)
file(GLOB_RECURSE sources_Custom Custom/*.cpp Custom/*.h)
file(GLOB_RECURSE sources_Commands Commands/*.cpp Commands/*.h)
#file(GLOB_RECURSE sources_Examples Examples/*.cpp Examples/*.h)
file(GLOB_RECURSE sources_Events Events/*.cpp Events/*.h)
file(GLOB_RECURSE sources_Pandaria Pandaria/*.cpp Pandaria/*.h)
file(GLOB_RECURSE sources_Pet Pet/*.cpp Pet/*.h)
file(GLOB_RECURSE sources_OutdoorPvP OutdoorPvP/*.cpp OutdoorPvP/*.h)
file(GLOB_RECURSE sources_Spells Spells/*.cpp Spells/*.h)
file(GLOB_RECURSE sources_ScriptLoader ScriptLoader/*.cpp ScriptLoader/*.h)
file(GLOB_RECURSE sources_World World/*.cpp World/*.h)

source_group(CMake FILES CMakeLists.txt)
source_group(Battlegrounds FILES ${sources_Battlegrounds})
source_group(PrecompiledHeaders FILES ${sources_PrecompiledHeaders})
source_group(EasternKingdoms FILES ${sources_EasternKingdoms})
source_group(Kalimdor FILES ${sources_Kalimdor})
source_group(Outland FILES ${sources_Outland})
source_group(Northrend FILES ${sources_Northrend})
source_group(Maelstrom FILES ${sources_Maelstrom})
source_group(OutdoorPvP FILES ${sources_OutdoorPvP})
source_group(Commands FILES ${sources_Commands})
#source_group(Examples FILES ${sources_Examples})
source_group(Events FILES ${sources_Events})
source_group(Pandaria FILES ${sources_Pandaria})
source_group(Pet FILES ${sources_Pet})
source_group(Spells FILES ${sources_Spells})
source_group(ScriptLoader FILES ${sources_ScriptLoader})
source_group(World FILES ${sources_World})

message(STATUS "SCRIPT PREPARATIONS")
include(Spells/CMakeLists.txt)

include(Commands/CMakeLists.txt)
include(Battlegrounds/CMakeLists.txt)

#include(Examples/CMakeLists.txt)
set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  ${sources_ScriptLoader}
)

if(SCRIPTS)
  include(Custom/CMakeLists.txt)
  include(World/CMakeLists.txt)
  include(OutdoorPvP/CMakeLists.txt)
  include(EasternKingdoms/CMakeLists.txt)
  include(Kalimdor/CMakeLists.txt)
  include(Outland/CMakeLists.txt)
  include(Northrend/CMakeLists.txt)
  include(Maelstrom/CMakeLists.txt)
  include(Events/CMakeLists.txt)
  include(Pandaria/CMakeLists.txt)
  include(Pet/CMakeLists.txt)
endif()

message(STATUS "SCRIPT PREPARATION COMPLETE")
message("")

if (ELUNA)
  set(scripts_STAT_SRCS
    ${scripts_STAT_SRCS}
  )
  include_directories(
    PRIVATE
      ${CMAKE_SOURCE_DIR}/dep/lualib
      ${CMAKE_SOURCE_DIR}/src/server/game/LuaEngine 
  )  
endif()

if( USE_SCRIPTPCH AND MSVC )
  set(scripts_STAT_SRCS
    ${scripts_STAT_SRCS}
  )
endif()

add_library(scripts STATIC 
  ${scripts_STAT_SRCS} 
)

target_include_directories(scripts
  PUBLIC
    ${CMAKE_BINARY_DIR}
    boost
    common
  PRIVATE   
    game
    fmt 
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
    ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
    ${CMAKE_SOURCE_DIR}/src/server/database/Database
    ${CMAKE_SOURCE_DIR}/src/server/database/Logging    
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
)

target_link_libraries(scripts
  PUBLIC
    game)

if(USE_MODULES)
	add_dependencies(scripts modules)
	target_link_libraries(scripts
		PRIVATE
		modules)
    target_compile_definitions(scripts PRIVATE USE_MODULES)
endif()

if (ELUNA)
  add_dependencies(scripts lua)
  target_link_libraries(scripts
    PRIVATE
      lua
  )
endif()

add_dependencies(scripts revision.h)

# Generate precompiled header
if (USE_SCRIPTPCH)
  target_precompile_headers(scripts PRIVATE ${scripts_STAT_PCH_HDR}) 
endif()
