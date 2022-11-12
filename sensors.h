#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>
#include <QTimer>

#include <QtSensors/QStepCounterSensor>
#include <QtSensors/QHrmSensor>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    virtual ~Logger() {};
private slots:
    void displayOn(QString displayState);
    void recordHeartrate();
    void recordGPS();
    void recordStepcounter();

    void setupRecordHeartrate();
    void setupRecordGPS();
    void setupRecordStepcounter();
private:
    QDateTime hrmLastTime;
    QDateTime stepsLastTime;
    QDBusInterface *m_iface;
    int hrmInterval;
    int gpsInterval;
    int stepsInterval;
    QTimer *hrmTimer;
    QTimer *gpsTimer;
    QTimer *stepsTimer;
    QHrmSensor *heartrateSensor ;
    QStepCounterSensor *stepcounterSensor;
};

#endif // LOGGER_H
