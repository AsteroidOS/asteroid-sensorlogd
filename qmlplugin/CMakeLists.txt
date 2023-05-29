add_library(
    sensorlogdqmlplugin
    sensorlogdqmlplugin.cpp
    sensorlogdqmlplugin.h
    stepsDataLoader.cpp
    stepsDataLoader.h
    loggerSettings.cpp
    loggerSettings.h
    hrGraph.cpp
    hrGraph.h
)

target_link_libraries(sensorlogdqmlplugin
	Qt5::Qml Qt5::Quick Qt5::DBus
)

install(TARGETS sensorlogdqmlplugin
	DESTINATION ${INSTALL_QML_IMPORT_DIR}/org/asteroid/sensorlogd)
install(FILES qmldir
	DESTINATION ${INSTALL_QML_IMPORT_DIR}/org/asteroid/sensorlogd)
