# This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( USE_COREPCH )
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB_RECURSE sources_DataStores DataStores/*.cpp DataStores/*.h)
file(GLOB_RECURSE sources_Dynamic Dynamic/*.cpp Dynamic/*.h)
file(GLOB_RECURSE sources_Networking Networking/*.cpp Networking/*.h)
file(GLOB_RECURSE sources_Packets Packets/*.cpp Packets/*.h)
file(GLOB_RECURSE sources_Realms Realms/*.cpp Realms/*.h)
file(GLOB_RECURSE sources_Threading Threading/*.cpp Threading/*.h)
file(GLOB_RECURSE sources_Utilities Utilities/*.cpp Utilities/*.h)
file(GLOB sources_localdir *.cpp *.h)

source_group(CMake FILES CMakeLists.txt)
source_group(DataStores FILES ${sources_DataStores})
source_group(Dynamic FILES ${sources_Dynamic})
source_group(Networking FILES ${sources_Networking})
source_group(Packets FILES ${sources_Packets})
source_group(Realms FILES ${sources_Realms})
source_group(Threading FILES ${sources_Threading})
source_group(Utilities FILES ${sources_Utilities})
source_group(localdir FILES ${sources_localdir})

#
# Build shared sourcelist
#

if (USE_COREPCH)
  set(shared_STAT_PCH_HDR PrecompiledHeaders/sharedPCH.h)
endif()

set(shared_STAT_SRCS
  ${shared_STAT_SRCS}
  ${sources_DataStores}
  ${sources_Dynamic}
  ${sources_Networking}
  ${sources_Packets}
  ${sources_Realms}
  ${sources_Threading}
  ${sources_Utilities}
  ${sources_localdir}
)

add_library(shared STATIC
  ${shared_STAT_SRCS}
)

target_include_directories(shared
  PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStores
    ${CMAKE_CURRENT_SOURCE_DIR}/Dynamic
    ${CMAKE_CURRENT_SOURCE_DIR}/Dynamic/LinkedReference
    ${CMAKE_CURRENT_SOURCE_DIR}/Networking
    ${CMAKE_CURRENT_SOURCE_DIR}/Packets
    ${CMAKE_CURRENT_SOURCE_DIR}/Realms
    ${CMAKE_CURRENT_SOURCE_DIR}/Threading
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities      
    boost
  PRIVATE
    common
    database
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${CMAKE_SOURCE_DIR}/src/server/database/Database
)

target_link_libraries(shared
  PRIVATE
    common
  PUBLIC
    database
)


# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(shared PRIVATE ${shared_STAT_PCH_HDR}) 
endif ()
