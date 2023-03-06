#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>
#include <QTimer>
#include <QString>

#include "sensorPlugins/stepCounter.h"
#include "sensorPlugins/heartrateSensor.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    virtual ~Logger() {};
private slots:
    void displayOn(QString displayState);

private:
    QDBusInterface *m_iface;
    bool heartrateSensorEnabled = true;
    HeartrateSensorPlugin *m_heartrateSensor;
    bool stepCounterEnabled = true;
    StepCounterPlugin *m_stepCounter;
};
void writeReadingToFile(QString data, QString filename);

#endif // LOGGER_H
