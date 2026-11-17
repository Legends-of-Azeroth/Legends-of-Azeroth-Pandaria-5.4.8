# Copyright (C) 2011-2016 Project SkyFire <http://www.projectskyfire.org/>
# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE mmap_gen_sources *.cpp *.h)

add_executable(mmaps_generator ${mmap_gen_sources})

target_include_directories(mmaps_generator 
  PUBLIC
    ${CMAKE_BINARY_DIR}
  PRIVATE  
    zlib
    bzip2
    g3dlite	 
    common
    shared
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
    ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
    ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
    ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
)

target_link_libraries(mmaps_generator
  PRIVATE
    common
    shared
    g3dlib
    Recast
    Detour
    bzip2
    zlib
    boost
    threads
    ${CMAKE_DL_LIBS}
)

if( UNIX )
  install(TARGETS mmaps_generator DESTINATION bin)
elseif( WIN32 )
  install(TARGETS mmaps_generator DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
