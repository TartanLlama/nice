include(CMakeFindDependencyMacro)
find_dependency(range-v3 REQUIRED)
find_dependency(fmt REQUIRED)
find_dependency(OpenSSL REQUIRED)
find_dependency(ZLIB REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/NiceTargets.cmake")