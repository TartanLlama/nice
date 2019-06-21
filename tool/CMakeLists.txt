add_executable(Nice nice.cpp command/command.cpp)
target_link_libraries(Nice PRIVATE Nice::LibNice range-v3)
set_target_properties(Nice PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if (MSVC)
  target_compile_options(Nice PRIVATE "/permissive-")
endif()

set_target_properties(Nice
  PROPERTIES OUTPUT_NAME nice)

include(GNUInstallDirs)
install(TARGETS Nice
    EXPORT NiceTargets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
