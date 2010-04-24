set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTTEST 1)
include(${QT_USE_FILE})

set(models_SOURCES user.cpp)
set(models_MOC_HEADERS user.h)

qt4_wrap_cpp(models_MOC_SOURCES ${models_MOC_HEADERS})
add_library(models STATIC ${models_SOURCES} ${models_MOC_SOURCES})

set(tester_SOURCES
    main.cpp)
set(tester_MOC_HEADERS
    main.h)

qt4_wrap_cpp(tester_MOC_SOURCES ${tester_MOC_HEADERS})
add_executable(tester ${tester_SOURCES} ${tester_MOC_SOURCES})
target_link_libraries(tester qdjango models ${QT_LIBRARIES})
