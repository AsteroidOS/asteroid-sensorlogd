#include <QSettings>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QtSensors/QStepCounterSensor>
#include <QtSensors/QHrmSensor>
#include <QDebug>

#include "sensors.h"

Logger::Logger(QObject *parent)  :
    QObject(parent){
    m_iface = new QDBusInterface("com.nokia.mce","/com/nokia/mce/signal", "com.nokia.mce.signal", QDBusConnection::systemBus());
    QSettings settings;
    if (true) { //add check for HRM
        hrmInterval = settings.value("hrmInterval",60000).toInt();

        heartrateSensor = new QHrmSensor(this);
        connect(heartrateSensor,SIGNAL(readingChanged()),this,SLOT(recordHeartrate()));

        qDebug() << "heartrate sensor is enabled. interval is (ms) " << hrmInterval;
        hrmTimer = new QTimer(this);
        connect(hrmTimer,SIGNAL(timeout()),this,SLOT(setupRecordHeartrate()));
        hrmTimer->setSingleShot(true);
        hrmTimer->start(hrmInterval);
        hrmLastTime = QDateTime::currentDateTime();
    }
    if (true) { //add check for HRM
        stepsInterval = settings.value("stepsInterval",60000).toInt();
        qDebug() << "step counter sensor is enabled. interval is (ms) " << stepsInterval;
        stepsTimer = new QTimer(this);
        connect(stepsTimer,SIGNAL(timeout()),this,SLOT(recordStepcounter()));
        stepsTimer->setSingleShot(true);
        stepsTimer->start(stepsInterval);
        stepsLastTime = QDateTime::currentDateTime();
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
    if (true) { //check for hrm
        uint elapsed = currTime - hrmLastTime.toMSecsSinceEpoch();
        qDebug() << "time until next hrm recording" << hrmTimer->remainingTime() << " elapsed = " << elapsed << " currTime " << currTime << " hrmLastTime" << hrmLastTime.toMSecsSinceEpoch();
        if (elapsed > hrmInterval) { //if too much time has passed, reset the timer and record heartrate
            recordHeartrate();
            hrmLastTime = QDateTime::currentDateTime();
        } else { //otherwise, restart the timer and compensate for time spent in suspend
            hrmTimer->start(hrmInterval - elapsed);
        }
    }
    if (true) { //check for stepcounter
        uint elapsed = currTime - stepsLastTime.toMSecsSinceEpoch();
        qDebug() << "time until next steps recording" << stepsTimer->remainingTime() << " elapsed = " << elapsed << " currTime " << currTime << " stepsLastTime" << stepsLastTime.toMSecsSinceEpoch();
        if (elapsed > stepsInterval) { //if too much time has passed, reset the timer and record heartrate
            recordStepcounter();
            stepsLastTime = QDateTime::currentDateTime();
        } else { //otherwise, restart the timer and compensate for time spent in suspend
            stepsTimer->start(stepsInterval - elapsed);
        }
    }
}

void Logger::setupRecordHeartrate() {
    qDebug() << "heartrate interval recording";
    hrmTimer->start(hrmInterval);
    heartrateSensor->start();
}

void Logger::recordHeartrate() {
    qDebug() << "bpm update received";
    int bpm = heartrateSensor->reading()->bpm();
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << bpm << heartrateSensor->status() << heartrateSensor->isActive();
    if ((bpm == 0) || (heartrateSensor->status() < 3)) {
        return;
    }
    QFile file("out.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    file.seek(file.size());
    QTextStream out(&file);
    out << QDateTime::currentSecsSinceEpoch() << " : " << bpm << "\n";
    qDebug() << "wrote to file";
    heartrateSensor->stop();
    file.close();
}

void Logger::setupRecordGPS() {
    QTimer::singleShot(hrmInterval, this, SLOT(recordHeartrate()));
    QHrmSensor* heartrateSensor = new QHrmSensor(this); //ok GPS doesn't work like this, this gonna need a custom solution. QtLocation to the rescue, or something :/
    QFile file("out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << QDateTime::currentSecsSinceEpoch() << " : " << heartrateSensor->reading();
    file.close();
    //create the GPS location
    //open the file we're recording into
    //wait for the sensor to finish setting up
    //read location
    //format
    //save to file
}

void Logger::recordGPS() {
}

void Logger::setupRecordStepcounter() {
    qDebug() << "stepcounter interval recording";
    hrmTimer->start(hrmInterval);
    QFile file("out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    stepcounterSensor->start();
    QTextStream out(&file);
    while(/*stepcounterSensor->reading()->steps() == 0*/false) {
        qDebug() << "waiting for reading, currently " << stepcounterSensor->reading()->steps() << stepcounterSensor->isActive();
    }
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << stepcounterSensor->reading()->steps() << stepcounterSensor->isActive();
    out << QDateTime::currentSecsSinceEpoch() << " : " << stepcounterSensor->reading(); //reading() like this doesn't do anything useful. We need to wait for sensor to initialise and feed us back a reading
    stepcounterSensor->stop();
    file.close();
}

void Logger::recordStepcounter() {
}
