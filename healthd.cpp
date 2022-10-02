#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>
#include "sensors.h"

int main(int argc, char **argv)
{
    QCoreApplication qcoreapp(argc, argv);
    if (!QDBusConnection::systemBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus system bus.\n");
        return 3;
    }
    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n");
        return 2;
    }
    Logger sensorsLogger;
    QCoreApplication::setOrganizationName("asteroid");
    QCoreApplication::setOrganizationDomain("asteroidos.org");
    QCoreApplication::setApplicationName("healthd");
    qDebug() << "healthd started";
    qcoreapp.exec();
    return 0;
}
