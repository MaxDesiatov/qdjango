# Common definitions

QT += sql

QDJANGO_INCLUDE_DIR = $$PWD/src
QDJANGO_LIBRARY_DIR = $$PWD/src
QDJANGO_LIBRARY_NAME = qdjango

# Libraries for apps which use QDjango
symbian {
    # Symbian needs a .lib extension to recognise the library as static
    QDJANGO_LIBS = -l$${QDJANGO_LIBRARY_NAME}.lib
} else {
    QDJANGO_LIBS = -l$${QDJANGO_LIBRARY_NAME}
}

