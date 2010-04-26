set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTTEST 1)
include(${QT_USE_FILE})

set(models_SOURCES models.cpp)
set(models_MOC_HEADERS models.h)

qt4_wrap_cpp(models_MOC_SOURCES ${models_MOC_HEADERS})
add_library(models STATIC ${models_SOURCES} ${models_MOC_SOURCES})

set(qdjango-console_SOURCES
    console.cpp
    script.cpp)

add_definitions(-DQT_STATICPLUGIN)
add_executable(qdjango-console ${qdjango-console_SOURCES})
target_link_libraries(qdjango-console qdjango models ${QT_QTSQL_LIBRARY} ${QT_QTSCRIPT_LIBRARY})

set(qdjango-tests_SOURCES
    main.cpp)
set(qdjango-tests_MOC_HEADERS
    main.h)

qt4_wrap_cpp(qdjango-tests_MOC_SOURCES ${qdjango-tests_MOC_HEADERS})
add_executable(qdjango-tests ${qdjango-tests_SOURCES} ${qdjango-tests_MOC_SOURCES})
target_link_libraries(qdjango-tests qdjango models ${QT_QTSQL_LIBRARY} ${QT_QTTEST_LIBRARY})
