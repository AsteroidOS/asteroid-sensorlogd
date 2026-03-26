cmake_minimum_required(VERSION 3.10.0)

project(sensorlogd
    VERSION 1.0.0
    DESCRIPTION "A sensor logger daemon for AsteroidOS oriented towards health tracking"
)

find_package(PkgConfig)

include(FeatureSummary)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core DBus Qml Sensors REQUIRED)
pkg_check_modules(mlite6 REQUIRED Mlite6)

add_subdirectory(daemon)
add_subdirectory(qmlplugin)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
