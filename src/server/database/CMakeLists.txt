# This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

file(GLOB_RECURSE sources_Database Database/*.cpp Database/*.h Database/Implementation/*.cpp Database/Implementation/*.h  )
file(GLOB_RECURSE sources_Logging Logging/*.cpp Logging/*.h)
file(GLOB_RECURSE sources_Updater Updater/*.cpp Logging/*.h)
file(GLOB sources_localdir *.cpp *.h)

source_group(CMake FILES CMakeLists.txt)
source_group(Database FILES ${sources_Database})
source_group(Logging FILES ${sources_Logging})
source_group(Updater FILES ${sources_Updater})
source_group(localdir FILES ${sources_localdir})

#
# Build database sourcelist
#

if (USE_COREPCH)
  set(database_STAT_PCH_HDR PrecompiledHeaders/databasePCH.h)
endif()

set(database_STAT_SRCS
  ${database_STAT_SRCS}
  ${sources_Database}
  ${sources_Logging}
  ${sources_Updater}
  ${sources_localdir}
)

add_library(database STATIC
  ${database_STAT_SRCS}
)

target_include_directories(database
  PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Database
    ${CMAKE_CURRENT_SOURCE_DIR}/Database/Implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/Logging
    ${CMAKE_CURRENT_SOURCE_DIR}/Updater
  PRIVATE
    common
    shared
    ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic
    mysql
    boost
)

target_link_libraries(database
  PRIVATE
    common
    mysql
    boost
)

# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(database PRIVATE ${database_STAT_PCH_HDR}) 
endif ()
