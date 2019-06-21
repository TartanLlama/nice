add_library(LibNice index.cpp database/database.cpp database/tree.cpp hash.cpp config.cpp)
add_library(Nice::LibNice ALIAS LibNice)
set_target_properties(LibNice
  PROPERTIES OUTPUT_NAME nice)

set_target_properties(LibNice PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(LibNice PRIVATE fmt::fmt range-v3 ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})

target_include_directories(LibNice
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

if (MSVC)
  target_compile_options(LibNice PUBLIC "/permissive-")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(LibNice PUBLIC stdc++fs)
endif()

include(GNUInstallDirs)
install(TARGETS LibNice
    EXPORT NiceTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
