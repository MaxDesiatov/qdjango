cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)
if(NOT QDJANGO_DONT_USE_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
