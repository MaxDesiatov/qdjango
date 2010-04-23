cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTTEST 1)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(qdjango_SOURCES
    model.cpp
    query.cpp)
set(qdjango_MOC_HEADERS
    model.h)

qt4_wrap_cpp(qdjango_MOC_SOURCES ${qdjango_MOC_HEADERS})
add_library(qdjango STATIC ${qdjango_SOURCES} ${qdjango_MOC_SOURCES})
set_target_properties(qdjango PROPERTIES SOVERSION 0)
target_link_libraries(qdjango ${QT_LIBRARIES})

set(tester_SOURCES
    tests.cpp
    tests/user.cpp)
set(tester_MOC_HEADERS
    tests.h
    tests/user.h)

qt4_wrap_cpp(tester_MOC_SOURCES ${tester_MOC_HEADERS})
add_executable(tester ${tester_SOURCES} ${tester_MOC_SOURCES})
target_link_libraries(tester qdjango)

