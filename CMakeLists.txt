cmake_minimum_required(VERSION 3.10)
project(Nice VERSION 0.1.0 LANGUAGES CXX)

option(NICE_ENABLE_TESTS "Enable tests." ON)
option(NICE_ENABLE_DOCS "Enable documentation." OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(range-v3 REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

find_path(OPTIONAL_INCLUDE_DIR tl/optional.hpp)
if (OPTIONAL_INCLUDE_DIR STREQUAL "OPTIONAL_INCLUDE_DIR-NOTFOUND")
    message(FATAL_ERROR "Could not find tl-optional")
endif()

include(CTest)
add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(tests)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("NiceConfigVersion.cmake"
	VERSION ${Nice_VERSION}
	COMPATIBILITY SameMajorVersion
	)
install(FILES "cmake/NiceConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/NiceConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Nice)

install(EXPORT NiceTargets
  FILE
    NiceTargets.cmake
  NAMESPACE
    Nice::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/Nice
)

if(NICE_ENABLE_DOCS)
  add_subdirectory(docs)
endif()



