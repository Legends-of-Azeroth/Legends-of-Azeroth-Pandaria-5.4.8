# Copyright (C) 2011-2016 Project SkyFire <http://www.projectskyfire.org/
# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions(--no-warnings)
elseif( MSVC )
  add_definitions(/W0)
endif()

if(SERVERS AND NOT NOJEM)
    add_subdirectory(jemalloc)  
endif()

add_subdirectory(zlib)
add_subdirectory(g3dlite)
add_subdirectory(recastnavigation)
add_subdirectory(fmt)
add_subdirectory(SFMT)
add_subdirectory(openssl)
add_subdirectory(boost)
add_subdirectory(threads)

if(SERVERS)
  add_subdirectory(gsoap)
  add_subdirectory(readline)
  add_subdirectory(mysql)  
  add_subdirectory(utf8cpp)  
endif()

if(TOOLS)
  add_subdirectory(StormLib)
  add_subdirectory(bzip2)  
endif()
if (ELUNA)
  add_subdirectory(lualib)
endif()
