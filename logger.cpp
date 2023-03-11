#include <QSettings>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

#include "logger.h"

#include "sensorPlugins/stepCounter.h"
#include "sensorPlugins/heartrateSensor.h"

Logger::Logger(QObject *parent)  :
    QObject(parent){
    m_iface = new QDBusInterface("com.nokia.mce","/com/nokia/mce/signal", "com.nokia.mce.signal", QDBusConnection::systemBus());
    QSettings settings;

//intialise HRM
    if (heartrateSensorEnabled) { //add check for HRM
        m_heartrateSensor = new HeartrateSensorPlugin(this,settings.value("stepsInterval",600000).toInt());
    }

//initialise step counter
    if (stepCounterEnabled) { //add check for step sensor
        m_stepCounter = new StepCounterPlugin(this,settings.value("stepsInterval",600000).toInt());
    }

    if(!m_iface->isValid()) {
	    qDebug() << "interface is not valid";
	    qDebug() << m_iface->lastError();
    }
    if(connect(m_iface, SIGNAL(display_status_ind(QString)), this, SLOT(displayOn(QString)))) { //this fires when the display turns on
	    qDebug() << "healthd connected display_status signal to slot";
    }
    qDebug() << "healthd sensors logger initialised";
}

void Logger::displayOn(QString displayState) {
    if (displayState == "off")
        return;
    qDebug() << "display on detected";
    uint currTime = QDateTime::currentMSecsSinceEpoch();

    if (heartrateSensorEnabled) {
        m_heartrateSensor->timeUpdate();
    }

    if (stepCounterEnabled) {
        m_stepCounter->timeUpdate();
    }
}

void writeReadingToFile(QString data, QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    file.seek(file.size());
    QTextStream out(&file);
    out << data;
    file.close();
}

QString getLineFromFile(int lineNumber, QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return "0 : 0";
    }
    QTextStream inStream(&file);
    QString line;
    int i;
    while(!inStream.atEnd() & (i < lineNumber | i < 0))
    {
        line = inStream.readLine();
        qDebug() << line;
        i++;
    }
    file.close();
    return line;
}
