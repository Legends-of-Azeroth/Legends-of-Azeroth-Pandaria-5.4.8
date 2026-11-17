include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
    ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
    ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography/Authentication
    ${CMAKE_SOURCE_DIR}/src/server/shared/Database
    ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
    ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
    ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic/CountedReference
    ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic/LinkedReference
    ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic
    ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
    ${CMAKE_SOURCE_DIR}/src/server/shared/Memory
    ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
    ${CMAKE_SOURCE_DIR}/src/server/shared/Policies
    ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
    ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
    ${CMAKE_SOURCE_DIR}/dep/utf8cpp
    ${Boost_INCLUDE_DIRS}
    ${MYSQL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

add_executable(updater
    App.cpp
)

target_link_libraries(updater
    shared
    ${MYSQL_LIBRARY}
    ${Boost_LIBRARIES}
)

if (UNIX)
    target_link_libraries(updater
        "pthread"
    )
endif()

if (WIN32)
    add_custom_command(TARGET updater
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/updater.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
    )
endif()

if (UNIX)
    install(TARGETS updater DESTINATION bin)
    install(FILES updater.conf.dist DESTINATION etc)
elseif (WIN32)
    install(TARGETS updater DESTINATION "${CMAKE_INSTALL_PREFIX}") 
    install(FILES updater.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()