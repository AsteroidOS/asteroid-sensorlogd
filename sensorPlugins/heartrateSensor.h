#ifndef HEARTRATESENSOR_H
#define HEARTRATESENSOR_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>
#include <QTimer>

#include <QtSensors/QHrmSensor>

class HeartrateSensorPlugin : public QObject
{
    Q_OBJECT
public:
    explicit HeartrateSensorPlugin(QObject *parent = 0, int initInterval = 600000);
    virtual ~HeartrateSensorPlugin() {};

    void timeUpdate();

public slots:
    void triggerRecording();

private slots:
    void finishRecording();

private:
    QDateTime lastRecordTime;
    int interval;
    QTimer *recordIntervalTimer;
    QHrmSensor *hrmSensor;
};

#endif // HEARTRATESENSOR_H
