#ifndef PASSENGER_H
#define PASSENGER_H

#include <QObject>
#include <QString>

class Passenger : public QObject {
    Q_OBJECT
    Q_PROPERTY(int passengerId READ passengerId WRITE setPassengerId)
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString idCard READ idCard WRITE setIdCard)

public:
    explicit Passenger(QObject *parent = nullptr) : QObject(parent), m_passengerId(0) {}

    // Getter
    int passengerId() const { return m_passengerId; }
    QString orderId() const { return m_orderId; }
    QString name() const { return m_name; }
    QString idCard() const { return m_idCard; }

    // Setter
    void setPassengerId(int passengerId) { m_passengerId = passengerId; }
    void setOrderId(const QString &orderId) { m_orderId = orderId; }
    void setName(const QString &name) { m_name = name; }
    void setIdCard(const QString &idCard) { m_idCard = idCard; }

private:
    int m_passengerId;
    QString m_orderId;
    QString m_name;
    QString m_idCard;
};

#endif // PASSENGER_H
