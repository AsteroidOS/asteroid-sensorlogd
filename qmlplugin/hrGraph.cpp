/*
 * Copyright (C) 2023 Arseniy Movshev <dodoradio@outlook.com>
 *               2017 Florent Revest <revestflo@gmail.com>
 * All rights reserved.
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hrGraph.h"
#include "stepsDataLoader.h"

#include <QPainter>
#include <QDate>
#include <QFile>
#include <QVector>
#include <QSettings>
#include <QStandardPaths>

HrGraph::HrGraph()
{
    setFlag(ItemHasContents, true);
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    loadGraphData();
}

void HrGraph::paint(QPainter *painter)
{
    if (!m_fileLoadStatus) {
        return;
    }
    int j = m_filedata.count();
    QPointF points[j];
    if (!graphRelative) {
        minHrValue = 0;
    }
    float valueDelta = maxHrValue - minHrValue;
    float timeDelta = maxTime - minTime;
    float calculatedValue = 0;
    float calculatedTimeSeconds = 0;
    for(int i = 0; i < j; i++) {
        calculatedTimeSeconds = (m_filedata.at(i).time - minTime)/timeDelta;
        calculatedValue = 1 - (m_filedata.at(i).value - minHrValue)/valueDelta;
        points[i] = QPointF(m_lineWidth + calculatedTimeSeconds*(width()-2*m_lineWidth), m_lineWidth + calculatedValue*(height()-2*m_lineWidth)); //these +2 -1 are here to make sure that the graph fits within the drawn area, as it will be clipped by qt if it doesn't.
    }
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidthF(m_lineWidth);
    pen.setColor(m_color);
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setPen(pen);
    painter->drawPolyline(points,j);
}

void HrGraph::loadGraphData(QDate date) {
    m_fileLoadStatus = false;
    QFile file(fileNameForDate(date, "heartrateSensor"));
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    m_fileLoadStatus = true;
    QTextStream inStream(&file);
    QString line;
    int i = 0;
    int currTime;
    int currValue;
    m_filedata.clear();
    while(!inStream.atEnd())
    {
        line = inStream.readLine();
        currTime = line.split(":")[0].toInt();
        currValue = line.split(":")[1].toInt();
        HrDatapoint datapoint;
        datapoint.time = currTime;
        datapoint.value = currValue;
        m_filedata.append(datapoint);
        if(i == 0) {
            minTime = currTime;
            maxHrValue = currValue;
            minHrValue = currValue;
        }
        if(currValue > maxHrValue) { maxHrValue = currValue; }
        if(currValue < minHrValue) { minHrValue = currValue; }
        i++;
    }
    maxTime = currTime;
    qDebug() << "heartrate graph file loading done";
    file.close();
}

void HrGraph::setLineColor(QColor color) {
    m_color = color;
    update();
}

QColor HrGraph::lineColor() {
    return m_color;
}

void HrGraph::setLineWidth(float width) {
    m_lineWidth = width;
    update();
}

float HrGraph::lineWidth() {
    return m_lineWidth;
}

int HrGraph::getMaxHrValue() {
    return maxHrValue;
}

int HrGraph::getMinHrValue() {
    return minHrValue;
}

QDateTime HrGraph::getMaxTime() {
    return QDateTime::fromSecsSinceEpoch(maxTime);
}

QDateTime HrGraph::getMinTime() {
    return QDateTime::fromSecsSinceEpoch(minTime);
}

bool HrGraph::relative() {
    return graphRelative;
}

void HrGraph::setRelative(bool newRelative) {
    graphRelative = newRelative;
}
