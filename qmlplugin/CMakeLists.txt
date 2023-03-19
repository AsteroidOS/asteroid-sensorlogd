add_library(
    sensorlogdqmlplugin
    sensorlogdqmlplugin.cpp
    sensorlogdqmlplugin.h
    stepsDataLoader.cpp
    stepsDataLoader.h
)

target_link_libraries(sensorlogdqmlplugin
	Qt5::Qml
)

install(TARGETS sensorlogdqmlplugin
	DESTINATION ${INSTALL_QML_IMPORT_DIR}/org/asteroid/sensorlogd)
install(FILES qmldir
	DESTINATION ${INSTALL_QML_IMPORT_DIR}/org/asteroid/sensorlogd)
