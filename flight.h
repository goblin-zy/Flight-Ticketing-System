#ifndef FLIGHT_H
#define FLIGHT_H

#include <QObject>
#include <QDateTime> // 替换QTime，引入QDateTime

class Flight : public QObject
{
    Q_OBJECT
    // 所有时间属性改为QDateTime
    Q_PROPERTY(int flightId READ flightId WRITE setFlightId)
    Q_PROPERTY(QString flightNumber READ flightNumber WRITE setFlightNumber)
    Q_PROPERTY(QString departureCity READ departureCity WRITE setDepartureCity)
    Q_PROPERTY(QString arrivalCity READ arrivalCity WRITE setArrivalCity)
    Q_PROPERTY(QDateTime departureTime READ departureTime WRITE setDepartureTime)
    Q_PROPERTY(QDateTime arrivalTime READ arrivalTime WRITE setArrivalTime)
    Q_PROPERTY(QDateTime checkinStartTime READ checkinStartTime WRITE setCheckinStartTime)
    Q_PROPERTY(QDateTime checkinEndTime READ checkinEndTime WRITE setCheckinEndTime)
    Q_PROPERTY(double price READ price WRITE setPrice)
    Q_PROPERTY(int leftTickets READ leftTickets WRITE setLeftTickets)

public:
    explicit Flight(QObject *parent = nullptr) : QObject(parent) {}

    // Getter + Setter 全部适配QDateTime
    int flightId() const { return m_flightId; }
    void setFlightId(int flightId) { m_flightId = flightId; }

    QString flightNumber() const { return m_flightNumber; }
    void setFlightNumber(const QString &flightNumber) { m_flightNumber = flightNumber; }

    QString departureCity() const { return m_departureCity; }
    void setDepartureCity(const QString &departureCity) { m_departureCity = departureCity; }

    QString arrivalCity() const { return m_arrivalCity; }
    void setArrivalCity(const QString &arrivalCity) { m_arrivalCity = arrivalCity; }

    QDateTime departureTime() const { return m_departureTime; }
    void setDepartureTime(const QDateTime &departureTime) { m_departureTime = departureTime; }

    QDateTime arrivalTime() const { return m_arrivalTime; }
    void setArrivalTime(const QDateTime &arrivalTime) { m_arrivalTime = arrivalTime; }

    QDateTime checkinStartTime() const { return m_checkinStartTime; }
    void setCheckinStartTime(const QDateTime &checkinStartTime) { m_checkinStartTime = checkinStartTime; }

    QDateTime checkinEndTime() const { return m_checkinEndTime; }
    void setCheckinEndTime(const QDateTime &checkinEndTime) { m_checkinEndTime = checkinEndTime; }

    double price() const { return m_price; }
    void setPrice(double price) { m_price = price; }

    int leftTickets() const { return m_leftTickets; }
    void setLeftTickets(int leftTickets) { m_leftTickets = leftTickets; }

private:
    // 成员变量同步改为QDateTime
    int m_flightId = 0;
    QString m_flightNumber;
    QString m_departureCity;
    QString m_arrivalCity;
    QDateTime m_departureTime;
    QDateTime m_arrivalTime;
    QDateTime m_checkinStartTime;
    QDateTime m_checkinEndTime;
    double m_price = 0.0;
    int m_leftTickets = 0;
};

#endif // FLIGHT_H
