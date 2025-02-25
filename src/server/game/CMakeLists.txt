# This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Accounts Accounts/*.cpp Accounts/*.h)
file(GLOB_RECURSE sources_Achievements Achievements/*.cpp Achievements/*.h)
file(GLOB_RECURSE sources_Addons Addons/*.cpp Addons/*.h)
file(GLOB_RECURSE sources_AI AI/*.cpp AI/*.h)
file(GLOB_RECURSE sources_Anticheat Anticheat/*.cpp Anticheat/*.h)
file(GLOB_RECURSE sources_AuctionHouse AuctionHouse/*.cpp AuctionHouse/*.h)
file(GLOB_RECURSE sources_AuctionHouseBot AuctionHouseBot/*.cpp AuctionHouse/*.h)
file(GLOB_RECURSE sources_Battlefield Battlefield/*.cpp Battlefield/*.h)
file(GLOB_RECURSE sources_Battlegrounds Battlegrounds/*.cpp Battlegrounds/*.h)
file(GLOB_RECURSE sources_BattlePay BattlePay/*.cpp BattlePay/*.h)
file(GLOB_RECURSE sources_BattlePet BattlePet/*.cpp BattlePet/*.h)
file(GLOB_RECURSE sources_BlackMarket BlackMarket/*.cpp BlackMarket/*.h)
file(GLOB_RECURSE sources_Calendar Calendar/*.cpp Calendar/*.h)
file(GLOB_RECURSE sources_Chat Chat/*.cpp Chat/*.h)
file(GLOB_RECURSE sources_Combat Combat/*.cpp Combat/*.h)
file(GLOB_RECURSE sources_Conditions Conditions/*.cpp Conditions/*.h)
file(GLOB_RECURSE sources_CustomTransmogrification CustomTransmogrification/*.cpp CustomTransmogrification/*.h)
file(GLOB_RECURSE sources_DataStores DataStores/*.cpp DataStores/*.h)
file(GLOB_RECURSE sources_DungeonFinding DungeonFinding/*.cpp DungeonFinding/*.h)
file(GLOB_RECURSE sources_Entities Entities/*.cpp Entities/*.h)
file(GLOB_RECURSE sources_Events Events/*.cpp Events/*.h)
file(GLOB_RECURSE sources_Globals Globals/*.cpp Globals/*.h)
file(GLOB_RECURSE sources_Grids Grids/*.cpp Grids/*.h)
file(GLOB_RECURSE sources_Groups Groups/*.cpp Groups/*.h)
file(GLOB_RECURSE sources_Guilds Guilds/*.cpp Guilds/*.h)
file(GLOB_RECURSE sources_Handlers Handlers/*.cpp Handlers/*.h)
file(GLOB_RECURSE sources_Instances Instances/*.cpp Instances/*.h)
file(GLOB_RECURSE sources_Loot Loot/*.cpp Loot/*.h)
file(GLOB_RECURSE sources_Mails Mails/*.cpp Mails/*.h)
file(GLOB_RECURSE sources_Maps Maps/*.cpp Maps/*.h)
file(GLOB_RECURSE sources_Miscellaneous Miscellaneous/*.cpp Miscellaneous/*.h)
file(GLOB_RECURSE sources_Motd Motd/*.cpp Motd/*.h)
file(GLOB_RECURSE sources_Movement Movement/*.cpp Movement/*.h)
file(GLOB_RECURSE sources_OutdoorPvP OutdoorPvP/*.cpp OutdoorPvP/*.h)
file(GLOB_RECURSE sources_Pools Pools/*.cpp Pools/*.h)
file(GLOB_RECURSE sources_PrecompiledHeaders PrecompiledHeaders/*.cpp PrecompiledHeaders/*.h)
file(GLOB_RECURSE sources_Quests Quests/*.cpp Quests/*.h)
file(GLOB_RECURSE sources_Reputation Reputation/*.cpp Reputation/*.h)
file(GLOB_RECURSE sources_Scenarios Scenarios/*.cpp Scenarios/*.h)
file(GLOB_RECURSE sources_Scripting Scripting/*.cpp Scripting/*.h)
file(GLOB_RECURSE sources_Server Server/*.cpp Server/*.h Server/Packets/*.h Server/Packets/*.cpp Server/Protocol/*.h Server/Protocol/*.cpp)
file(GLOB_RECURSE sources_Services Services/*.cpp Services/*.h)
file(GLOB_RECURSE sources_Skills Skills/*.cpp Skills/*.h)
file(GLOB_RECURSE sources_Spells Spells/*.cpp Spells/*.h)
file(GLOB_RECURSE sources_Texts Texts/*.cpp Texts/*.h)
file(GLOB_RECURSE sources_Time Time/*.cpp Time/*.h)
file(GLOB_RECURSE sources_Tools Tools/*.cpp Tools/*.h)
file(GLOB_RECURSE sources_Tickets Tickets/*.cpp Tickets/*.h)
file(GLOB_RECURSE sources_Vignette Vignette/*.cpp Vignette/*.h)
file(GLOB_RECURSE sources_Warden Warden/*.cpp Warden/*.h)
file(GLOB_RECURSE sources_Weather Weather/*.cpp Weather/*.h)
file(GLOB_RECURSE sources_World World/*.cpp World/*.h)

source_group(CMake FILES CMakeLists.txt)
source_group(Accounts FILES ${sources_Accounts})
source_group(Achievements FILES ${sources_Achievements})
source_group(Addons FILES ${sources_Addons})
source_group(AI FILES ${sources_AI})
source_group(Anticheat FILES ${sources_Anticheat})
source_group(AuctionHouse FILES ${sources_AuctionHouse})
source_group(AuctionHouseBot FILES ${sources_AuctionHouseBot})
source_group(Battlefield FILES ${sources_Battlefield})
source_group(Battlegrounds FILES ${sources_Battlegrounds})
source_group(BattlePay FILES ${sources_BattlePay})
source_group(BattlePet FILES ${sources_BattlePet})
source_group(BlackMarket FILES ${sources_BlackMarket})
source_group(Calendar FILES ${sources_Calendar})
source_group(Chat FILES ${sources_Chat})
source_group(Combat FILES ${sources_Combat})
source_group(Conditions FILES ${sources_Conditions})
source_group(CustomTransmogrification FILES ${sources_CustomTransmogrification})
source_group(DataStores FILES ${sources_DataStores})
source_group(DungeonFinding FILES ${sources_DungeonFinding})
source_group(Entities FILES ${sources_Entities})
source_group(Events FILES ${sources_Events})
source_group(Globals FILES ${sources_Globals})
source_group(Grids FILES ${sources_Grids})
source_group(Groups FILES ${sources_Groups})
source_group(Guilds FILES ${sources_Guilds})
source_group(Handlers FILES ${sources_Handlers})
source_group(Instances FILES ${sources_Instances})
source_group(Loot FILES ${sources_Loot})
source_group(Mails FILES ${sources_Mails})
source_group(Maps FILES ${sources_Maps})
source_group(Miscellaneous FILES ${sources_Miscellaneous})
source_group(Motd FILES ${sources_Motd})
source_group(Movement FILES ${sources_Movement})
source_group(OutdoorPvP FILES ${sources_OutdoorPvP})
source_group(Pools FILES ${sources_Pools})
source_group(PrecompiledHeaders FILES ${sources_PrecompiledHeaders})
source_group(Quests FILES ${sources_Quests})
source_group(Reputation FILES ${sources_Reputation})
source_group(Scenarios FILES ${sources_Scenarios})
source_group(Scripting FILES ${sources_Scripting})
source_group(Server FILES ${sources_Server})
source_group(Services FILES ${sources_Services})
source_group(Skills FILES ${sources_Skills})
source_group(Spells FILES ${sources_Spells})
source_group(Texts FILES ${sources_Texts})
source_group(Time FILES ${sources_Time})
source_group(Tools FILES ${sources_Tools})
source_group(Tickets FILES ${sources_Tickets})
source_group(Warden FILES ${sources_Warden})
source_group(Weather FILES ${sources_Weather})
source_group(World FILES ${sources_World})
# Create game-libary

if (USE_COREPCH)
  set(game_STAT_PCH_HDR PrecompiledHeaders/gamePCH.h)
endif ()

set(game_STAT_SRCS
  ${game_STAT_SRCS}
  ${sources_Accounts}
  ${sources_Achievements}
  ${sources_Addons}
  ${sources_AI}
  ${sources_Anticheat}
  ${sources_AuctionHouse}
  ${sources_AuctionHouseBot}
  ${sources_Battlefield}
  ${sources_Battlegrounds}
  ${sources_BattlePay}
  ${sources_BattlePet}
  ${sources_BlackMarket}
  ${sources_Calendar}
  ${sources_Chat}
  ${sources_Combat}
  ${sources_Conditions}
  ${sources_CustomTransmogrification}
  ${sources_DataStores}
  ${sources_DungeonFinding}
  ${sources_Entities}
  ${sources_Events}
  ${sources_Globals}
  ${sources_Grids}
  ${sources_Groups}
  ${sources_Guilds}
  ${sources_Handlers}
  ${sources_Instances}
  ${sources_Loot}
  ${sources_Mails}
  ${sources_Maps}
  ${sources_Miscellaneous}
  ${sources_Motd}
  ${sources_Movement}
  ${sources_OutdoorPvP}
  ${sources_Pools}
  ${sources_Quests}
  ${sources_Reputation}
  ${sources_Scenarios}
  ${sources_Scripting}
  ${sources_Server}
  ${sources_Services}
  ${sources_Skills}
  ${sources_Spells}
  ${sources_Texts}
  ${sources_Time}
  ${sources_Tools}
  ${sources_Tickets}
  ${sources_Vignette}
  ${sources_Warden}
  ${sources_Weather}
  ${sources_World}
  ${sources_Cfbg}
)

if (ELUNA)
  file(GLOB_RECURSE sources_LuaEngine LuaEngine/*.cpp LuaEngine/*.h)
  source_group(LuaEngine FILES ${sources_LuaEngine})
  set(game_STAT_SRCS
    ${game_STAT_SRCS}
    ${sources_LuaEngine}
  )
endif()

add_library(game STATIC
  ${game_STAT_SRCS}
)

target_include_directories(game
  PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Accounts
    ${CMAKE_CURRENT_SOURCE_DIR}/Achievements
    ${CMAKE_CURRENT_SOURCE_DIR}/Addons
    ${CMAKE_CURRENT_SOURCE_DIR}/AI
    ${CMAKE_CURRENT_SOURCE_DIR}/AI/CoreAI
    ${CMAKE_CURRENT_SOURCE_DIR}/AI/PlayerAI
    ${CMAKE_CURRENT_SOURCE_DIR}/AI/ScriptedAI
    ${CMAKE_CURRENT_SOURCE_DIR}/AI/SmartScripts
    ${CMAKE_CURRENT_SOURCE_DIR}/Anticheat
    ${CMAKE_CURRENT_SOURCE_DIR}/AuctionHouse
    ${CMAKE_CURRENT_SOURCE_DIR}/AuctionHouseBot
    ${CMAKE_CURRENT_SOURCE_DIR}/Battlefield
    ${CMAKE_CURRENT_SOURCE_DIR}/Battlefield/Zones
    ${CMAKE_CURRENT_SOURCE_DIR}/Battlegrounds
    ${CMAKE_CURRENT_SOURCE_DIR}/Battlegrounds/Zones
    ${CMAKE_CURRENT_SOURCE_DIR}/BattlePay
    ${CMAKE_CURRENT_SOURCE_DIR}/BattlePet
    ${CMAKE_CURRENT_SOURCE_DIR}/BlackMarket
    ${CMAKE_CURRENT_SOURCE_DIR}/Calendar
    ${CMAKE_CURRENT_SOURCE_DIR}/Chat
    ${CMAKE_CURRENT_SOURCE_DIR}/Chat/Channels
    ${CMAKE_CURRENT_SOURCE_DIR}/Combat
    ${CMAKE_CURRENT_SOURCE_DIR}/Conditions
    ${CMAKE_CURRENT_SOURCE_DIR}/CustomTransmogrification
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStores
    ${CMAKE_CURRENT_SOURCE_DIR}/DungeonFinding
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/AreaTrigger
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Creature
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Corpse
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DynamicObject
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/GameObject
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Item
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Item/Container
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Object
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Object/Updates
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Pet
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Player
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Totem
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Unit
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Vehicle
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Transport
    ${CMAKE_CURRENT_SOURCE_DIR}/Events
    ${CMAKE_CURRENT_SOURCE_DIR}/Globals
    ${CMAKE_CURRENT_SOURCE_DIR}/Grids/Cells
    ${CMAKE_CURRENT_SOURCE_DIR}/Grids/Notifiers
    ${CMAKE_CURRENT_SOURCE_DIR}/Grids
    ${CMAKE_CURRENT_SOURCE_DIR}/Groups
    ${CMAKE_CURRENT_SOURCE_DIR}/Guilds
    ${CMAKE_CURRENT_SOURCE_DIR}/Handlers
    ${CMAKE_CURRENT_SOURCE_DIR}/Instances
    ${CMAKE_CURRENT_SOURCE_DIR}/Loot
    ${CMAKE_CURRENT_SOURCE_DIR}/Mails
    ${CMAKE_CURRENT_SOURCE_DIR}/Maps
    ${CMAKE_CURRENT_SOURCE_DIR}/Miscellaneous
    ${CMAKE_CURRENT_SOURCE_DIR}/Motd
    ${CMAKE_CURRENT_SOURCE_DIR}/Movement
    ${CMAKE_CURRENT_SOURCE_DIR}/Movement/Spline
    ${CMAKE_CURRENT_SOURCE_DIR}/Movement/MovementGenerators
    ${CMAKE_CURRENT_SOURCE_DIR}/Movement/Waypoints
    ${CMAKE_CURRENT_SOURCE_DIR}/OutdoorPvP
    ${CMAKE_CURRENT_SOURCE_DIR}/Pools
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Quests
    ${CMAKE_CURRENT_SOURCE_DIR}/Reputation
    ${CMAKE_CURRENT_SOURCE_DIR}/Scenarios
    ${CMAKE_CURRENT_SOURCE_DIR}/Scripting
    ${CMAKE_CURRENT_SOURCE_DIR}/Server
    ${CMAKE_CURRENT_SOURCE_DIR}/Server/Packets
    ${CMAKE_CURRENT_SOURCE_DIR}/Server/Protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Skills
    ${CMAKE_CURRENT_SOURCE_DIR}/Spells
    ${CMAKE_CURRENT_SOURCE_DIR}/Spells/Auras
    ${CMAKE_CURRENT_SOURCE_DIR}/Texts
    ${CMAKE_CURRENT_SOURCE_DIR}/Time
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools
    ${CMAKE_CURRENT_SOURCE_DIR}/Tickets
    ${CMAKE_CURRENT_SOURCE_DIR}/Vignette
    ${CMAKE_CURRENT_SOURCE_DIR}/Warden
    ${CMAKE_CURRENT_SOURCE_DIR}/Warden/Modules
    ${CMAKE_CURRENT_SOURCE_DIR}/Weather
    ${CMAKE_CURRENT_SOURCE_DIR}/World    
    boost
    common
  PRIVATE
    zlib
    database
    shared
    g3dlite
    Detour
    Recast
)

if (ELUNA)
  target_include_directories(game
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/LuaEngine
    PRIVATE
      lua
  )

endif()

target_link_libraries(game
  PUBLIC
    Detour
    common
    database
    shared
  PRIVATE
    zlib
)

if(USE_MODULES)
    target_compile_definitions(game PRIVATE USE_MODULES)
endif()

if (ELUNA)
  add_dependencies(game lua)
  target_link_libraries(game
    PRIVATE
      lua
  )
  if( WIN32 )
    if ( MSVC )
      add_custom_command(TARGET game
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/lua_scripts/extensions/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/LuaEngine/extensions" "${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/lua_scripts/extensions/"
      )
    elseif ( MINGW )
      add_custom_command(TARGET game
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/lua_scripts/extensions/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/LuaEngine/extensions" "${CMAKE_BINARY_DIR}/bin/lua_scripts/extensions/"
      )
    endif()
  endif()      
endif()

add_dependencies(game revision.h)

# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(game PRIVATE ${game_STAT_PCH_HDR}) 
endif ()
