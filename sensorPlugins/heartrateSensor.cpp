#include <QDateTime>
#include <QTimer>
#include <QtSensors/QHrmSensor>
#include <QDebug>
#include <QString>

#include "../logger.h"

#include "heartrateSensor.h"

HeartrateSensorPlugin::HeartrateSensorPlugin(QObject *parent, int initInterval)  :
    QObject(parent){
    interval = initInterval;

    hrmSensor = new QHrmSensor(this);
    connect(hrmSensor,SIGNAL(readingChanged()),this,SLOT(finishRecording()));

    qDebug() << "heartrate sensor is enabled. interval is (ms) " << interval;
    recordIntervalTimer = new QTimer(this);
    connect(recordIntervalTimer,SIGNAL(timeout()),this,SLOT(triggerRecording()));
    recordIntervalTimer->setSingleShot(true);
    recordIntervalTimer->start(interval);
    lastRecordTime = QDateTime::currentDateTime();
}

void HeartrateSensorPlugin::timeUpdate() {
    uint elapsed = QDateTime::currentMSecsSinceEpoch() - lastRecordTime.toMSecsSinceEpoch();
    qDebug() << "time until next steps recording" << recordIntervalTimer->remainingTime() << " elapsed = " << elapsed << " lastRecordTime " << lastRecordTime.toMSecsSinceEpoch();
    if (elapsed > interval) { //if too much time has passed, reset the timer and record
        triggerRecording();
        lastRecordTime = QDateTime::currentDateTime();
    } else { //otherwise, restart the timer and compensate for time spent in suspend
        recordIntervalTimer->start(interval - elapsed);
    }
}

void HeartrateSensorPlugin::triggerRecording() {
    qDebug() << "heartrate interval recording";
    recordIntervalTimer->start(interval);
    hrmSensor->start();
}

void HeartrateSensorPlugin::finishRecording() {
    qDebug() << "bpm update received";
    int bpm = hrmSensor->reading()->bpm();
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << bpm << hrmSensor->status() << hrmSensor->isActive();
    if ((bpm == 0) || (hrmSensor->status() < 3)) {
        qDebug() << "hrm sensor accuracy insufficient. waiting.";
        return;
    }
    writeReadingToFile(QString::number(QDateTime::currentSecsSinceEpoch()) + " : " + QString::number(bpm) + "\n", "hrmOut.txt");
    hrmSensor->stop();
}
