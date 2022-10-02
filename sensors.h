#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>
#include <QTimer>

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
};

#endif // LOGGER_H
