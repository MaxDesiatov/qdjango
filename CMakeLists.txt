cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(qdjango_SOURCES
    model.cpp
    where.cpp)
set(qdjango_MOC_HEADERS
    model.h)

qt4_wrap_cpp(qdjango_MOC_SOURCES ${qdjango_MOC_HEADERS})
add_library(qdjango STATIC ${qdjango_SOURCES} ${qdjango_MOC_SOURCES})
set_target_properties(qdjango PROPERTIES SOVERSION 0)
target_link_libraries(qdjango ${QT_LIBRARIES})

add_subdirectory(tests)
