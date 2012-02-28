include(../qdjango.pri)

QT       += sql testlib

TARGET = tst_qvariantmaptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$QDJANGO_INCLUDE_DIR

SOURCES += tst_qvariantmaptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_LFLAGS += -F$$QDJANGO_LIBRARY_DIR

macx {
    CONFIG += x86
    LIBS += -framework qdjango
}

unix:!macx {
    LIBS += -L../src -lqdjango
}

win32:!win32-g++-4.6 {
    LIBS += $$QDJANGO_LIBRARY_DIR/qdjango.lib
}

win32-g++-4.6 {
    LIBS += $$QDJANGO_LIBRARY_DIR/libqdjango.a
}

