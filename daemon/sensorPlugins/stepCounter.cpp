/*
 * Copyright (C) 2023 Arseniy Movshev <dodoradio@outlook.com>
 * This file is part of sensorlogd, a sensor logger for the AsteroidOS smartwatch OS.
 *
 * sensorlogd is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * sensorlogd is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDateTime>
#include <QDate>
#include <QTime>
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

    QDateTime currDateTime = QDateTime::currentDateTime();

    setupFilePath(sensorPathPrefix);

    if (dayFileExists(sensorPathPrefix)) {
        QStringList lastLineData = fileGetPrevRecord(sensorPathPrefix);
        lastRecordTime = QDateTime::fromSecsSinceEpoch(lastLineData[0].toInt());
        if (stepcounterSensor->reading()->steps() == 0) {
            stepsOffset = -(lastLineData[1].toInt());
        } else {
            stepsOffset = - stepcounterSensor->reading()->steps() + (lastLineData[1].toInt());
        }
    } else {
        //if it's a new day, we 'reset' the counter. this is crude - we should really check for a boot here, since certain machines have capability of counting steps when powered down.
        stepsOffset = stepcounterSensor->reading()->steps();
    }
}

void StepCounterPlugin::timeUpdate() {
    QDateTime currDateTime = QDateTime::currentDateTime();
    uint elapsed = currDateTime.toMSecsSinceEpoch() - lastRecordTime.toMSecsSinceEpoch();
    qDebug() << "time until next steps recording" << recordIntervalTimer->remainingTime() << " elapsed = " << elapsed << " lastRecordTime " << lastRecordTime.toMSecsSinceEpoch();
    if (elapsed > interval) { //if too much time has passed, reset the timer and record
        triggerRecording();
    } else { //otherwise, restart the timer and compensate for time spent in suspend
        recordIntervalTimer->start(interval - elapsed);
    }
}

void StepCounterPlugin::triggerRecording() {
    qDebug() << "stepcounter interval recording";
    if (lastRecordTime.date() < QDate::currentDate()) {
        int steps = stepcounterSensor->reading()->steps();
        fileAddRecord(sensorPathPrefix,QString::number(steps - stepsOffset),QDateTime(QDate::currentDate().addDays(-1),QTime(23,59,59))); // this writes the current step count to the last second of the previous day
        stepcounterSensor->reading()->setSteps(0); // and then we reset the step counter
        stepsOffset = 0; // and, since the step counter has been reset, there's no need for an offset any more
        qDebug() << "date change detected; recorded " << steps << " to previous day and reset step counter hw";
    } else {
        int steps = stepcounterSensor->reading()->steps();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << steps << stepcounterSensor->isActive();
        //we probably ought to do some error checking here
        fileAddRecord(sensorPathPrefix,QString::number(steps - stepsOffset));
    }
    lastRecordTime = QDateTime::currentDateTime();
}
