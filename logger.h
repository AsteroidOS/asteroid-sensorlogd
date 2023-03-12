#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDate>
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
    void fileAddRecord(QString sensorPrefix, QString logdata, QDateTime recordTime = QDateTime::currentDateTime()); //adds a record to today's log file for the given sensor
    bool dayFileExists(QString sensorPrefix, QDateTime date = QDateTime::currentDateTime()); //check if today has a log file for the given sensor
    QStringList fileGetPrevRecord(QString sensorPrefix, QDateTime recordTime = QDateTime::currentDateTime()); //works backwards to find the last record in today's file before the given time - returns nothing if no file is found.
    QString fileNameForDate(QDate date, QString prefix);

#endif // LOGGER_H
