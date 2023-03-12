#include <QSettings>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
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

void fileAddRecord(QString sensorPrefix, QString logdata, QDateTime recordTime) { //adds a record to today's log file for the given sensor
    qDebug() << fileNameForDate(recordTime.date(), sensorPrefix);
    QFile file(fileNameForDate(recordTime.date(), sensorPrefix));
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    file.seek(file.size());
    QTextStream out(&file);
    out << QString::number(recordTime.currentSecsSinceEpoch()) + ":" + logdata + "\n";
    file.close();
}
bool dayFileExists(QString sensorPrefix, QDateTime dateTime) {
    return QFile::exists(fileNameForDate(dateTime.date(), sensorPrefix));
}

QStringList fileGetPrevRecord(QString sensorPrefix, QDateTime recordTime) {
    qDebug() << fileNameForDate(recordTime.date(), sensorPrefix);
    QFile file(fileNameForDate(recordTime.date(), sensorPrefix));
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return {0,0};
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
    return line.split(":");
}

QString fileNameForDate(QDate date, QString prefix) {
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/asteroid-healthloggerd/" + prefix + "/" + date.toString("yyyy-MM-dd.log");
}
