cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTSQL 1)
include(${QT_USE_FILE})

# Needed for plugin support
if(UNIX)
  if(NOT APPLE)
    add_definitions(-fPIC)
  endif(NOT APPLE)
endif(UNIX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Until binary stability is reached, build static libraries
set(QDJANGO_STATIC 1)
if(QDJANGO_STATIC)
  set(LIBRARY_TYPE STATIC)
else()
  set(LIBRARY_TYPE SHARED)
endif()

# Optional debugging
if(QDJANGO_DEBUG_SQL)
  add_definitions(-DQDJANGO_DEBUG_SQL=1)
endif(QDJANGO_DEBUG_SQL)

# QDjango core library
set(qdjango_SOURCES
    QDjango.cpp
    QDjangoModel.cpp
    QDjangoQuerySet.cpp
    QDjangoWhere.cpp)
set(qdjango_MOC_HEADERS
    QDjango_p.h
    QDjangoModel.h)
qt4_wrap_cpp(qdjango_MOC_SOURCES ${qdjango_MOC_HEADERS})
add_library(qdjango ${LIBRARY_TYPE} ${qdjango_SOURCES} ${qdjango_MOC_SOURCES})
set_target_properties(qdjango PROPERTIES SOVERSION 0)
target_link_libraries(qdjango ${QT_QTSQL_LIBRARY} ${QT_QTCORE_LIBRARY})

# QDjango http library
set(qdjango-http_SOURCES
    QDjangoHttpController.cpp
    QDjangoHttpRequest.cpp
    QDjangoHttpResponse.cpp
    QDjangoHttpServer.cpp)
set(qdjango-http_MOC_HEADERS
    QDjangoHttpResponse.h
    QDjangoHttpServer_p.h)
qt4_wrap_cpp(qdjango-http_MOC_SOURCES ${qdjango-http_MOC_HEADERS})
add_library(qdjango-http ${LIBRARY_TYPE} ${qdjango-http_SOURCES} ${qdjango-http_MOC_SOURCES})
set_target_properties(qdjango-http PROPERTIES SOVERSION 0)
target_link_libraries(qdjango-http ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY})

# QDjango script library
add_library(qdjango-script ${LIBRARY_TYPE} QDjangoScript.cpp)
set_target_properties(qdjango-script PROPERTIES SOVERSION 0)
target_link_libraries(qdjango-script qdjango ${QT_QTSCRIPT_LIBRARY} ${QT_QTCORE_LIBRARY})
