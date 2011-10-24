include(../qdjango.pri)

TEMPLATE = lib
CONFIG += staticlib

# Target definition
TARGET = $$QDJANGO_LIBRARY_NAME
VERSION = $$QDJANGO_VERSION

# Plugins
DEFINES += QT_STATICPLUGIN
HEADERS += \
    QDjango.h \
    QDjango_p.h \
    QDjangoModel.h \
    QDjangoQuerySet.h \
    QDjangoQuerySet_p.h \
    QDjangoWhere.h
SOURCES += \
    QDjango.cpp \
    QDjangoModel.cpp \
    QDjangoQuerySet.cpp \
    QDjangoWhere.cpp

