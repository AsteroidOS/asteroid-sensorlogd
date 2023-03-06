#ifndef STEPCOUNTER_H
#define STEPCOUNTER_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>
#include <QTimer>

#include <QtSensors/QStepCounterSensor>

class StepCounterPlugin : public QObject
{
    Q_OBJECT
public:
    explicit StepCounterPlugin(QObject *parent = 0, int initInterval = 600000);
    virtual ~StepCounterPlugin() {};

    void timeUpdate();

public slots:
    void triggerRecording();

private:
    QDateTime lastRecordTime;
    int interval;
    QTimer *recordIntervalTimer;
    QStepCounterSensor *stepcounterSensor;
};

#endif // STEPCOUNTER_H
