# Common definitions

QDJANGO_INCLUDE_DIR = $$PWD
QDJANGO_LIBRARY_DIR = $$PWD
QDJANGO_LIBRARY_NAME = qdjango

# Libraries for apps which use QDjango
symbian {
    # Symbian needs a .lib extension to recognise the library as static
    QDJANGO_LIBS = -L$$QDJANGO_LIBRARY_DIR -l$${QDJANGO_LIBRARY_NAME}.lib
} else {
    QDJANGO_LIBS = -L$$QDJANGO_LIBRARY_DIR -l$${QDJANGO_LIBRARY_NAME}
}

