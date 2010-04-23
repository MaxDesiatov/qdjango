set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTTEST 1)
include(${QT_USE_FILE})

set(tester_SOURCES
    main.cpp
    user.cpp)
set(tester_MOC_HEADERS
    main.h
    user.h)

qt4_wrap_cpp(tester_MOC_SOURCES ${tester_MOC_HEADERS})
add_executable(tester ${tester_SOURCES} ${tester_MOC_SOURCES})
target_link_libraries(tester qdjango ${QT_LIBRARIES})

