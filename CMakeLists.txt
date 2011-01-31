cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
