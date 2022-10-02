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
        qDebug() << "heartrate sensor is enabled. interval is (ms) " << hrmInterval;
        hrmTimer = new QTimer(this);
        connect(hrmTimer,SIGNAL(timeout()),this,SLOT(recordHeartrate()));
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

void Logger::recordHeartrate() {
    qDebug() << "heartrate interval recording";
    hrmTimer->start(hrmInterval);
    QHrmSensor* heartrateSensor = new QHrmSensor(this);
    QFile file("out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file";
        return;
    }
    heartrateSensor->start();
    QTextStream out(&file);
    while(heartrateSensor->reading()->bpm() == 0) {
        qDebug() << "waiting for reading, currently " << heartrateSensor->reading()->bpm() << heartrateSensor->status() << heartrateSensor->isActive();
    }
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") << " : " << heartrateSensor->reading()->bpm() << heartrateSensor->status() << heartrateSensor->isActive();
    out << QDateTime::currentSecsSinceEpoch() << " : " << heartrateSensor->reading(); //reading() like this doesn't do anything useful. We need to wait for sensor to initialise and feed us back a reading
    heartrateSensor->stop();
    file.close();
}

void Logger::recordGPS() {
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

void Logger::recordStepcounter() {
    qDebug() << "stepcounter interval recording";
    hrmTimer->start(hrmInterval);
    QStepCounterSensor* stepcounterSensor = new QStepCounterSensor(this);
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
