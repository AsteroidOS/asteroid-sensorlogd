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
#include <QSettings>
#include <QDBusInterface>
#include <QPointF>

#include "weightDataLoader.h"
#include "../common.h"

WeightDataLoader::WeightDataLoader() : QObject()
{
    m_iface = new QDBusInterface("org.asteroid.sensorlogd.logger","/org/asteroid/sensorlogd/logger","", QDBusConnection::sessionBus(), this);
    if (!m_iface->isValid()) {
        qDebug()<<"interface is not valid";
    } else {
        qDebug()<<"interface is valid";
    }
}

QVariant WeightDataLoader::getTodayData() {
    return getDataForDate(QDate::currentDate());
}

QVariant WeightDataLoader::getDataForDate(QDate date) {
    QList<QPointF> m_filedata;
    QFile file(fileNameForDate(date, "weight"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return QVariant::fromValue(m_filedata);
    }
    QTextStream inStream(&file);
    QString line;
    while (!inStream.atEnd()) {
        line = inStream.readLine();
        QPointF point;
        point.setX(line.split(":")[0].toInt());
        point.setY(line.split(":")[1].toInt());
        m_filedata.append(point);
    }
    file.close();
    return QVariant::fromValue(m_filedata);
}

void WeightDataLoader::triggerDaemonRecording() {
    m_iface->call("triggerRecording");
}
