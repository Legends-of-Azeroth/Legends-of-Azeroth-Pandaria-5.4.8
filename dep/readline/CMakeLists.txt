add_library(readline INTERFACE)

if( UNIX )
  find_package(Readline REQUIRED)

  target_link_libraries(readline
    INTERFACE
      Readline::Readline)
endif()
