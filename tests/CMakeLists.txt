set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Catch2 REQUIRED)

add_executable(nice_tests main.cpp index.cpp)
target_link_libraries(nice_tests Nice::LibNice Catch2::Catch2)
add_test(NAME nice COMMAND nice_tests)
