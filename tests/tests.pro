include(../qdjango.pri)

QT       += sql testlib

QT       -= gui

TARGET = tst_qvariantmaptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$QDJANGO_INCLUDE_DIR

SOURCES += tst_qvariantmaptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_LFLAGS += -F$$QDJANGO_LIBRARY_DIR
macx {
    LIBS += -framework qdjango
}

win32 {
    LIBS += $$QDJANGO_LIBRARY_DIR/qdjango.lib
}
