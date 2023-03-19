/*
 * Copyright (C) 2023 Arseniy Movshev <dodoradio@outlook.com>
 * This file is part of sensorlogd, a sensor logger for the AsteroidOS smartwatch OS.
 *
 * sensorlogd is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * sensorlogd is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QStandardPaths>

#include "stepsDataLoader.h"

StepsDataLoader::StepsDataLoader() : QObject()
{
}

int StepsDataLoader::getTodayData() { // This is obvious garbage. This should really be abstracted and cached, so that every page doesn't have to reload the file from scratch.
// The intention is to also add graph functionality at some point. The graph will be simplifying the data before loading it in - it would be worth caching the simplified data when it comes to that as well.
    QFile file(fileNameForDate(QDate::currentDate(), "stepCounter"));
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return 0;
    }
    QTextStream inStream(&file);
    QString line;
    int i;
    while(!inStream.atEnd())
    {
        line = inStream.readLine();
        qDebug() << line;
        i++;
    }
    file.close();
    return line.split(":")[1].toInt();
}

QString fileNameForDate(QDate date, QString prefix) {
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/asteroid-healthloggerd/" + prefix + "/" + date.toString("yyyy-MM-dd.log");
}
