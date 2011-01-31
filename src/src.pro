include(../qdjango.pri)

TEMPLATE = lib

CONFIG += staticlib

# Target definition
TARGET = $$QDJANGO_LIBRARY_NAME
VERSION = $$QDJANGO_VERSION
DESTDIR = $$QDJANGO_LIBRARY_DIR

# Plugins
DEFINES += QT_STATICPLUGIN
HEADERS += \
    QDjango.h \
    QDjango_p.h \
    QDjangoModel.h \
    QDjangoQuerySet.h \
    QDjangoWhere.h
SOURCES += \
    QDjango.cpp \
    QDjangoModel.cpp \
    QDjangoQuerySet.cpp \
    QDjangoWhere.cpp


