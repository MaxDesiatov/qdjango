set(QT_USE_QTNETWORK 1)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})

# needed for win32
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

add_library(qtscript_qdjango_test SHARED qdjango_test.cpp)
target_link_libraries(qtscript_qdjango_test qdjango qdjango-models ${QT_LIBRARIES})

#install(TARGETS
#    qtscript_qdjango_test
#	LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}/script
#	RUNTIME DESTINATION ${PLUGIN_INSTALL_DIR}/script)
