#ifndef FLIGHT_H
#define FLIGHT_H

#include <QObject>
#include <QDate>
#include <QTime>
#include <QString>

class Flight : public QObject
{
    Q_OBJECT
    // 严格对应数据库字段 + API返回字段
    Q_PROPERTY(int flightId READ flightId WRITE setFlightId)
    Q_PROPERTY(QString flightNumber READ flightNumber WRITE setFlightNumber)
    Q_PROPERTY(QString departureCity READ departureCity WRITE setDepartureCity)
    Q_PROPERTY(QString arrivalCity READ arrivalCity WRITE setArrivalCity)
    Q_PROPERTY(QDate date READ date WRITE setDate)                // 航班日期（单独字段）
    Q_PROPERTY(QTime departureTime READ departureTime WRITE setDepartureTime) // 出发时间（仅时分）
    Q_PROPERTY(QTime arrivalTime READ arrivalTime WRITE setArrivalTime)       // 到达时间（仅时分）
    Q_PROPERTY(QString departureAirport READ departureAirport WRITE setDepartureAirport)
    Q_PROPERTY(QString arrivalAirport READ arrivalAirport WRITE setArrivalAirport)
    Q_PROPERTY(QTime checkinStartTime READ checkinStartTime WRITE setCheckinStartTime)
    Q_PROPERTY(QTime checkinEndTime READ checkinEndTime WRITE setCheckinEndTime)
    Q_PROPERTY(int price READ price WRITE setPrice)              // 价格（分，整数）
    Q_PROPERTY(QString airlineCompany READ airlineCompany WRITE setAirlineCompany)
    Q_PROPERTY(QString status READ status WRITE setStatus)
    Q_PROPERTY(int leftTickets READ leftTickets WRITE setLeftTickets)

public:
    explicit Flight(QObject *parent = nullptr) : QObject(parent) {}

    // Getter方法（严格对应字段名）
    int flightId() const { return m_flightId; }
    QString flightNumber() const { return m_flightNumber; }
    QString departureCity() const { return m_departureCity; }
    QString arrivalCity() const { return m_arrivalCity; }
    QDate date() const { return m_date; }
    QTime departureTime() const { return m_departureTime; }
    QTime arrivalTime() const { return m_arrivalTime; }
    QString departureAirport() const { return m_departureAirport; }
    QString arrivalAirport() const { return m_arrivalAirport; }
    QTime checkinStartTime() const { return m_checkinStartTime; }
    QTime checkinEndTime() const { return m_checkinEndTime; }
    int price() const { return m_price; }
    QString airlineCompany() const { return m_airlineCompany; }
    QString status() const { return m_status; }
    int leftTickets() const { return m_leftTickets; }

    // Setter方法
    void setFlightId(int flightId) { m_flightId = flightId; }
    void setFlightNumber(const QString &flightNumber) { m_flightNumber = flightNumber; }
    void setDepartureCity(const QString &departureCity) { m_departureCity = departureCity; }
    void setArrivalCity(const QString &arrivalCity) { m_arrivalCity = arrivalCity; }
    void setDate(const QDate &date) { m_date = date; }
    void setDepartureTime(const QTime &departureTime) { m_departureTime = departureTime; }
    void setArrivalTime(const QTime &arrivalTime) { m_arrivalTime = arrivalTime; }
    void setDepartureAirport(const QString &departureAirport) { m_departureAirport = departureAirport; }
    void setArrivalAirport(const QString &arrivalAirport) { m_arrivalAirport = arrivalAirport; }
    void setCheckinStartTime(const QTime &checkinStartTime) { m_checkinStartTime = checkinStartTime; }
    void setCheckinEndTime(const QTime &checkinEndTime) { m_checkinEndTime = checkinEndTime; }
    void setPrice(int price) { m_price = price; }
    void setAirlineCompany(const QString &airlineCompany) { m_airlineCompany = airlineCompany; }
    void setStatus(const QString &status) { m_status = status; }
    void setLeftTickets(int leftTickets) { m_leftTickets = leftTickets; }

private:
    // 成员变量严格对应数据库列名
    int m_flightId = 0;
    QString m_flightNumber;
    QString m_departureCity;
    QString m_arrivalCity;
    QDate m_date;
    QTime m_departureTime;
    QTime m_arrivalTime;
    QString m_departureAirport;
    QString m_arrivalAirport;
    QTime m_checkinStartTime;
    QTime m_checkinEndTime;
    int m_price = 0;          // 单位：分
    QString m_airlineCompany;
    QString m_status;
    int m_leftTickets = 0;
};

#endif // FLIGHT_H
