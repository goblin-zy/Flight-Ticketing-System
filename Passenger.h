#ifndef PASSENGER_H
#define PASSENGER_H
#include <QObject>
#include <QString>

class Passenger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString idCard READ idCard WRITE setIdCard)

public:
    explicit Passenger(QObject *parent = nullptr) : QObject(parent) {}

    QString orderId() const { return m_orderId; }
    QString name() const { return m_name; }
    QString idCard() const { return m_idCard; }

    void setOrderId(const QString &orderId) { m_orderId = orderId; }
    void setName(const QString &name) { m_name = name; }
    void setIdCard(const QString &idCard) { m_idCard = idCard; }

private:
    QString m_orderId;    // 关联订单ID
    QString m_name;       // 乘客姓名
    QString m_idCard;     // 身份证号
};

#endif // PASSENGER_H
