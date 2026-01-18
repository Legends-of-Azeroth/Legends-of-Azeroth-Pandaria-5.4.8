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

# Set build-directive (used in core to tell which buildtype we used)
add_compile_options(-D_BUILD_DIRECTIVE="$<CONFIG>")
#add_compile_options(-fno-delete-null-pointer-checks)

if(TRINITY_SYSTEM_PROCESSOR MATCHES "x86|amd64")
  add_compile_options(-DHAVE_SSE2 -D__SSE2__)
  message(STATUS "GCC: SFMT enabled, SSE2 flags forced")
endif()

set(COMPILER_FLAGS "")

if (WITH_SANITIZER)
  set(COMPILER_FLAGS "${COMPILER_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize-recover=address -fsanitize-address-use-after-scope")
endif()

if (BUILD_DEPLOY AND NOT WITH_COREDEBUG)
  set(COMPILER_FLAGS "${COMPILER_FLAGS} -march=native -fno-strict-aliasing -ffunction-sections -fdata-sections")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections " CACHE INTERNAL "EXE_LINKER_FLAGS" FORCE) #-s -w
  if (NOT INSTALL_PREFIX)
    set(INSTALL_PREFIX "/server/wow/horizon")
  endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")

if( WITH_WARNINGS )
  add_compile_options(-Wall -Wfatal-errors -Wextra)
  message(STATUS "GCC: All warnings enabled")
else()
  add_compile_options(--no-warnings)
  message(STATUS "GCC: All warnings disabled")
endif()

if( WITH_COREDEBUG )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
  message(STATUS "GCC: Debug-flags set (-g3)")
endif()
