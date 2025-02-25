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

option(SERVERS          "Build worldserver and authserver"                            1)
option(SCRIPTS          "Build core with scripts included"                            1)
option(TOOLS            "Build map/vmap/mmap extraction/assembler tools"              0)
option(ELUNA            "Build Eluna Lua Engine"                                      1)
option(USE_SCRIPTPCH    "Use precompiled headers when compiling scripts"              1)
option(USE_COREPCH      "Use precompiled headers when compiling servers"              1)
option(WITH_WARNINGS    "Show all warnings during compile"                            0)
option(WITH_COREDEBUG   "Include additional debug-code in core"                       0)
option(WITH_SANITIZER   "Build with AddressSanitizer"                                 0)
option(AUTH_SERVER      "Build authserver"                                            1)
option(UPDATER          "Build updater"                                               0)
option(USE_MODULES		"Use modules system"										  1)
if (UNIX)
option(BUILD_DEPLOY     "Option of a build for deployment"                            1)
endif()
option(BUILD_DEV        "Experimental build for development under Windows"            0)