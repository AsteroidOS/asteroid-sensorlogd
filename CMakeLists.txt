cmake_minimum_required(VERSION 3.10.0)
project(sensorlogd
    VERSION 1.0.0
    DESCRIPTION "A sensor logger daemon for AsteroidOS oriented towards health tracking"
)

find_package(ECM REQUIRED NO_MODULE)
find_package(AsteroidApp REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ASTEROID_MODULE_PATH})

include(FeatureSummary)
include(GNUInstallDirs)
include(ECMFindQmlModule)
include(ECMGeneratePkgConfigFile)
include(AsteroidCMakeSettings)

include(AsteroidCMakeSettings)

find_package(Qt5 COMPONENTS Core DBus Qml Sensors REQUIRED)
find_package(Mlite5 MODULE REQUIRED)

add_subdirectory(daemon)
add_subdirectory(qmlplugin)
