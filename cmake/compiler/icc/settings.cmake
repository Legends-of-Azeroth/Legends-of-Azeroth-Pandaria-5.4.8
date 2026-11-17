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

if(PLATFORM EQUAL 32)
  add_compile_options(-axSSE2)
else()
  add_compile_options(-xSSE2)
endif()

if( WITH_WARNINGS )
  add_compile_options(-w1)
  message(STATUS "ICC: All warnings enabled")
endif()

if( WITH_COREDEBUG )
  add_compile_options(-g)
  message(STATUS "ICC: Debug-flag set (-g)")
endif()
