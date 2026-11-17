# This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

########### authserver ###############

file(GLOB_RECURSE sources_Authentication Authentication/*.cpp Authentication/*.h)
file(GLOB_RECURSE sources_PrecompiledHeaders PrecompiledHeaders/*.cpp PrecompiledHeaders/*.h)
file(GLOB_RECURSE sources_Realms Realms/*.cpp Realms/*.h)
file(GLOB_RECURSE sources_Server Server/*.cpp Server/*.h)
file(GLOB sources_Localdir *.cpp *.h)

source_group(CMake FILES CMakeLists.txt)
source_group(Authentication FILES ${sources_Authentication})
source_group(PrecompiledHeaders FILES ${sources_PrecompiledHeaders})
source_group(Realms FILES ${sources_Realms})
source_group(Server FILES ${sources_Server})
source_group(localdir FILES ${sources_Localdir})

if (USE_COREPCH)
  set(authserver_PCH_HDR PrecompiledHeaders/authPCH.h)
endif()

set(authserver_SRCS
  ${authserver_SRCS}
  ${sources_Authentication}
  ${sources_Realms}
  ${sources_Server}
  ${sources_Localdir}
)

if( WIN32 )
  set(authserver_SRCS
    ${authserver_SRCS}
    ${sources_windows_Debugging}
  )
  if ( MSVC )
    set(authserver_SRCS
      ${authserver_SRCS}
      authserver.rc
    )
  endif ()
endif()

add_executable(authserver
  ${authserver_SRCS}
)

target_include_directories(authserver
  PUBLIC
    ${CMAKE_BINARY_DIR}
  PRIVATE
    common 
    database
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/src/server/shared/Networking    
    ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
    ${CMAKE_SOURCE_DIR}/src/server/shared/Realms
    ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
    ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Authentication
    ${CMAKE_CURRENT_SOURCE_DIR}/Server
    openssl
    mysql
    boost
    ${CMAKE_SOURCE_DIR}/dep/fmt/include
)

add_dependencies(authserver revision.h)

if( NOT WIN32 )
  set_target_properties(authserver PROPERTIES
    COMPILE_DEFINITIONS _TRINITY_REALM_CONFIG="${CONF_DIR}/authserver.conf"
  )
endif()

target_link_libraries(authserver
  common
  shared
  database
  jemalloc
  mysql
  openssl
  threads
  ${CMAKE_DL_LIBS}
  boost
)

if( WIN32 )
  if ( MSVC )
    add_custom_command(TARGET authserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/authserver.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
    )
  elseif ( MINGW )
    add_custom_command(TARGET authserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/authserver.conf.dist ${CMAKE_BINARY_DIR}/bin/
    )
  endif()
endif()

if( UNIX )
  install(TARGETS authserver DESTINATION bin)
  install(FILES  authserver.conf.dist DESTINATION ${CONF_DIR})
elseif( WIN32 )
  install(TARGETS authserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES authserver.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(authserver PRIVATE ${authserver_PCH_HDR}) 
endif()
