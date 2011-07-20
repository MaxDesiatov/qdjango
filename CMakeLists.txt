cmake_minimum_required(VERSION 2.6)

# Project and version
project(QDjango)
set(QDJANGO_VERSION 0.1.0)

# Source package
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${QDJANGO_VERSION}")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/.git/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/.gitmodules")
list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}")
include(CPack)

find_package(Qt4 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)
if(NOT QDJANGO_DONT_USE_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
