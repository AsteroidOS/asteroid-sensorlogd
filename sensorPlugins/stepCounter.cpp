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
    QStringList lastLineData = getLineFromFile(-1,fileName).split(" : ");
    lastRecordTime = QDateTime::fromSecsSinceEpoch(lastLineData[0].toInt());
    QDateTime currDateTime = QDateTime::currentDateTime();
    qDebug() << "last record time: " << lastLineData[0] << " last record steps: " << lastLineData[1];
    if (lastRecordTime.date() < currDateTime.date()) { //if it's a new day, we 'reset' the counter. this is crude - we should really check for a boot here, since certain machines have capability of counting steps when powered down.
        stepsOffset = stepcounterSensor->reading()->steps();
    } else if (stepcounterSensor->reading()->steps() == 0) { //otherwise, keep counting from the last point. though we also need to check if we're running on the same boot as the previous recording, otherwise we may go into negatives.
        stepsOffset = -(lastLineData[1].toInt());
    }
    //and here we need to load in an appropriate offset. but this is a difficult topic. it would be nice really useful to figure out whether the daemon is running the first time for a given boot, in order to figure out whether we need to offset the sensor or take the first value as an actual data point. this gets even more messed up by catfish and other watches with their own rogue sensor management systems... maybe we could do the 'freshness' detection by creating a file in /tmp
    //alternatively, we just assume that steps should only be counted when the daemon is running. hence, we set the offset to [current sensor reading] - [last reading in today's log] and
}

void StepCounterPlugin::timeUpdate() {
    QDateTime currDateTime = QDateTime::currentDateTime();
    if (lastRecordTime.date() < currDateTime.date()) {
        stepsOffset = stepcounterSensor->reading()->steps(); //this 'resets' the reading whenever the screen is first turned on after midnight. this means that, in the morning, the step count will always be zero, but steps taken just before midnight are still counted and not discarded.
    }
    uint elapsed = currDateTime.toMSecsSinceEpoch() - lastRecordTime.toMSecsSinceEpoch();
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
    writeReadingToFile(QString::number(QDateTime::currentSecsSinceEpoch()) + " : " + QString::number(steps - stepsOffset) + "\n", fileName);
}
