cmake_minimum_required(VERSION 3.10.0)
project(healthd
    VERSION 1.0.0
    DESCRIPTION "A health tracker daemon for AsteroidOS"
)

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ASTEROID_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt5 COMPONENTS Core DBus Qml Positioning Sensors REQUIRED)


add_executable(healthd
    healthd.cpp
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
install(TARGETS healthd)
# configure_file(
#     "org.asteroid.steroid.trackrecorder.service.in"
#     "org.asteroid.steroid.trackrecorder.service"
# )
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.asteroid.steroid.trackrecorder.service DESTINATION /usr/lib/systemd/user)
