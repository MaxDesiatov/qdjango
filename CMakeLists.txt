cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTSQL 1)
include(${QT_USE_FILE})

# Enable debugging
#add_definitions(-DQDJANGO_DEBUG_SQL=1)

# Needed for plugin support
if(UNIX)
  if(NOT APPLE)
    add_definitions(-fPIC)
  endif(NOT APPLE)
endif(UNIX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(qdjango_SOURCES
    model.cpp
    queryset.cpp
    where.cpp)
set(qdjango_MOC_HEADERS
    model.h)

qt4_wrap_cpp(qdjango_MOC_SOURCES ${qdjango_MOC_HEADERS})
add_library(qdjango STATIC ${qdjango_SOURCES} ${qdjango_MOC_SOURCES})
set_target_properties(qdjango PROPERTIES SOVERSION 0)
target_link_libraries(qdjango ${QT_QTSQL_LIBRARY} ${QT_QTCORE_LIBRARY})

add_executable(qdjango-console console.cpp)
target_link_libraries(qdjango-console qdjango ${QT_LIBRARIES})

add_subdirectory(script)
add_subdirectory(tests)
