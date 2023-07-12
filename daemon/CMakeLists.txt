add_executable(sensorlogd
    sensorlogd.cpp
    logger.cpp
    logger.h
    sensorPlugins/stepCounter.cpp
    sensorPlugins/stepCounter.h
    sensorPlugins/heartrateSensor.cpp
    sensorPlugins/heartrateSensor.h
    ../common.cpp
    ../common.h
)
set_target_properties(sensorlogd PROPERTIES AUTOMOC ON)
#add_compile_definitions(Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR=${Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR})
target_link_libraries(sensorlogd PRIVATE Qt5::Core Qt5::DBus Qt5::Qml Qt5::Sensors)
install(TARGETS sensorlogd DESTINATION bin)
install(FILES systemd/asteroid-sensorlogd.service
		DESTINATION /usr/lib/systemd/user)
