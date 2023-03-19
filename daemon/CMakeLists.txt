add_executable(healthd
    sensorlogd.cpp
    logger.cpp
    logger.h
    sensorPlugins/stepCounter.cpp
    sensorPlugins/stepCounter.h
    sensorPlugins/heartrateSensor.cpp
    sensorPlugins/heartrateSensor.h
)
set_target_properties(healthd PROPERTIES AUTOMOC ON)
#add_compile_definitions(Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR=${Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR})
target_link_libraries(healthd PRIVATE Qt5::Core Qt5::DBus Qt5::Qml Qt5::Positioning Qt5::Sensors)
