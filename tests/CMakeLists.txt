set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTTEST 1)
include(${QT_USE_FILE})

# test models
set(qdjango-models_SOURCES auth/models.cpp shares/models.cpp)
set(qdjango-models_MOC_HEADERS auth/models.h shares/models.h)

qt4_wrap_cpp(qdjango-models_MOC_SOURCES ${qdjango-models_MOC_HEADERS})
add_library(qdjango-models STATIC ${qdjango-models_SOURCES} ${qdjango-models_MOC_SOURCES})
target_link_libraries(qdjango-models qdjango)

# test program
set(qdjango-tests_SOURCES main.cpp auth/tests.cpp shares/tests.cpp)
set(qdjango-tests_MOC_HEADERS main.h auth/tests.h shares/tests.h)
if(QDJANGO_USE_NETWORK)
    add_definitions(-DUSE_NETWORK)
    list(APPEND qdjango-tests_SOURCES http.cpp)
    list(APPEND qdjango-tests_MOC_HEADERS http.h)
endif()
if(QDJANGO_USE_SCRIPT)
    add_definitions(-DUSE_SCRIPT)
    list(APPEND qdjango-tests_SOURCES script.cpp)
    list(APPEND qdjango-tests_MOC_HEADERS script.h)
endif()

qt4_wrap_cpp(qdjango-tests_MOC_SOURCES ${qdjango-tests_MOC_HEADERS})
add_executable(qdjango-tests ${qdjango-tests_SOURCES} ${qdjango-tests_MOC_SOURCES})
target_link_libraries(qdjango-tests qdjango qdjango-http qdjango-models qdjango-script ${QT_LIBRARIES})

