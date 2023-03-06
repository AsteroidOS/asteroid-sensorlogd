#include <QDateTime>
#include <QTimer>
#include <QtSensors/QStepCounterSensor>
#include <QDebug>
#include <QString>

#include "../logger.h"

#include "stepCounter.h"

StepCounterPlugin::StepCounterPlugin(QObject *parent, int initInterval)  :
    QObject(parent){
    interval = initInterval;

    stepcounterSensor = new QStepCounterSensor(this);
    stepcounterSensor->start();

    qDebug() << "step counter sensor is enabled. interval is (ms) " << interval;
    recordIntervalTimer = new QTimer(this);
    connect(recordIntervalTimer,SIGNAL(timeout()),this,SLOT(triggerRecording()));
    recordIntervalTimer->setSingleShot(true);
    recordIntervalTimer->start(interval);
    lastRecordTime = QDateTime::currentDateTime();
}

void StepCounterPlugin::timeUpdate() {
    uint elapsed = QDateTime::currentMSecsSinceEpoch() - lastRecordTime.toMSecsSinceEpoch();
    qDebug() << "time until next steps recording" << recordIntervalTimer->remainingTime() << " elapsed = " << elapsed << " lastRecordTime " << lastRecordTime.toMSecsSinceEpoch();
    if (elapsed > interval) { //if too much time has passed, reset the timer and record
        triggerRecording();
        lastRecordTime = QDateTime::currentDateTime();
    } else { //otherwise, restart the timer and compensate for time spent in suspend
        recordIntervalTimer->start(interval - elapsed);
    }
}

void StepCounterPlugin::triggerRecording() {
    qDebug() << "stepcounter interval recording";
    int steps = stepcounterSensor->reading()->steps();
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << steps << stepcounterSensor->isActive();
    //we probably ought to do some error checking here
    writeReadingToFile(QString::number(QDateTime::currentSecsSinceEpoch()) + " : " + QString::number(steps) + "\n", "stepsOut.txt");
}
