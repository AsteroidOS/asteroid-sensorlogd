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
#include <time.h>

#include "../logger.h"

#include "stepCounter.h"

StepCounterPlugin::StepCounterPlugin(QObject *parent, int initInterval, bool daemonFresh)  :
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
    while (!stepcounterSensor->isActive()) {}

    if (dayFileExists(sensorPathPrefix) && daemonFresh) {
        QStringList lastLineData = fileGetPrevRecord(sensorPathPrefix);
        lastRecordTime = QDateTime::currentDateTime();
        stepcounterSensor->reading()->setSteps(lastLineData[1].toInt() + stepcounterSensor->reading()->steps()); // we add the last recorded value from today to the current value. This 'recovers' the steps from between reboots. I'm not sure how this will work on catfish or medaka.
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
        fileAddRecord(sensorPathPrefix,QString::number(steps),QDateTime(QDate::currentDate().addDays(-1),QTime(23,59,59))); // this writes the current step count to the last second of the previous day
        stepcounterSensor->reading()->setSteps(0); // and then we reset the step counter
        qDebug() << "date change detected; recorded " << steps << " to previous day and reset step counter hw";
    } else {
        int steps = stepcounterSensor->reading()->steps();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << steps << stepcounterSensor->isActive();
        //we probably ought to do some error checking here
        fileAddRecord(sensorPathPrefix,QString::number(steps));
    }
    lastRecordTime = QDateTime::currentDateTime();
}
