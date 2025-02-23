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

file(GLOB_RECURSE sources_Asio Asio/*.cpp Asio/*.h)
file(GLOB_RECURSE sources_Collision Collision/*.cpp Collision/*.h Collision/Management/*.cpp Collision/Management/*.h Collision/Maps/*.cpp Collision/Maps/*.h Collision/Models/*.cpp Collision/Models/*.h)
file(GLOB_RECURSE sources_Configuration Configuration/*.cpp Configuration/*.h)
file(GLOB_RECURSE sources_Containers Containers/*.cpp Containers/*.h Containers/Utilities/*.cpp Containers/Utilities/*.h)
file(GLOB_RECURSE sources_Cryptography Cryptography/*.cpp Cryptography/*.h Cryptography/Authentication/*.cpp Cryptography/Authentication/*.h)
file(GLOB_RECURSE sources_DataStores DataStores/*.cpp DataStores/*.h)
file(GLOB_RECURSE sources_Hacks Hacks/*.cpp Hacks/*.h)
file(GLOB_RECURSE sources_IPLocation IPLocation/*.cpp IPLocation/*.h)
file(GLOB_RECURSE sources_Logging Logging/*.cpp Logging/*.h)
file(GLOB_RECURSE sources_Threading Threading/*.cpp Threading/*.h)
file(GLOB_RECURSE sources_Utilities Utilities/*.cpp Utilities/*.h)
file(GLOB sources_localdir *.cpp *.h)

source_group(CMake FILES CMakeLists.txt)
source_group(Asio FILES ${sources_Asio})
source_group(Collision FILES ${sources_Collision})
source_group(Configuration FILES ${sources_Configuration})
source_group(Containers FILES ${sources_Containers})
source_group(Cryptography FILES ${sources_Cryptography})
source_group(DataStores FILES ${sources_DataStores})
source_group(Hacks FILES ${sources_Hacks})
source_group(IPLocation FILES ${sources_IPLocation})
source_group(Logging FILES ${sources_Logging})
source_group(Threading FILES ${sources_Threading})
source_group(Utilities FILES ${sources_Utilities})

# Manually set sources for Debugging directory as we don't want to include WheatyExceptionReport in shared project
# It needs to be included both in authserver and worldserver for the static global variable to be properly initialized
# and to handle crash logs on windows
set(sources_Debugging Debugging/Errors.cpp Debugging/Errors.h)

if (USE_COREPCH)
  set(common_STAT_PCH_HDR PrecompiledHeaders/commonPCH.h)
endif()

set(common_STAT_SRCS
  ${common_STAT_SRCS}
  ${sources_Asio}
  ${sources_Collision}
  ${sources_Configuration}
  ${sources_Containers}
  ${sources_Cryptography}
  ${sources_DataStores}
  ${sources_Debugging}
  ${sources_Hacks}
  ${sources_IPLocation}
  ${sources_Logging} 
  ${sources_Threading} 
  ${sources_Utilities}
  ${sources_localdir}
)

add_library(common STATIC
  ${common_STAT_SRCS}
)

target_include_directories(common
  PUBLIC
    openssl
    boost
    fmt
    utf8cpp
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Asio
    ${CMAKE_CURRENT_SOURCE_DIR}/Collision
    ${CMAKE_CURRENT_SOURCE_DIR}/Collision/Management
    ${CMAKE_CURRENT_SOURCE_DIR}/Collision/Maps
    ${CMAKE_CURRENT_SOURCE_DIR}/Collision/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/Containers
    ${CMAKE_CURRENT_SOURCE_DIR}/Containers/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/Configuration
    ${CMAKE_CURRENT_SOURCE_DIR}/Cryptography
    ${CMAKE_CURRENT_SOURCE_DIR}/Cryptography/Authentication  
    ${CMAKE_CURRENT_SOURCE_DIR}/DataStores
    ${CMAKE_CURRENT_SOURCE_DIR}/Debugging
    ${CMAKE_CURRENT_SOURCE_DIR}/Hacks
    ${CMAKE_CURRENT_SOURCE_DIR}/IPLocation
    ${CMAKE_CURRENT_SOURCE_DIR}/Logging
    ${CMAKE_CURRENT_SOURCE_DIR}/Threading
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
    ${CMAKE_BINARY_DIR}
  PRIVATE
    sfmt
    ${CMAKE_SOURCE_DIR}/dep/utf8cpp
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(common
  PUBLIC
    sfmt
    fmt
    openssl
    boost
    g3dlib
    Detour
    utf8cpp
    jemalloc    
)

add_dependencies(common revision.h)

# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(common PRIVATE ${common_STAT_PCH_HDR}) 
endif ()
