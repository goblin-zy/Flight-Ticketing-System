#ifndef ORDERINFO_H
#define ORDERINFO_H

#include <QObject>
#include <QDateTime>

class OrderInfo : public QObject
{
    Q_OBJECT
    // orderId改为QString类型（适配ORD格式字符串）
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)
    Q_PROPERTY(int userId READ userId WRITE setUserId)
    Q_PROPERTY(int flightId READ flightId WRITE setFlightId)
    Q_PROPERTY(double totalPrice READ totalPrice WRITE setTotalPrice)
    Q_PROPERTY(QString status READ status WRITE setStatus)
    Q_PROPERTY(QDateTime createTime READ createTime WRITE setCreateTime)

public:
    explicit OrderInfo(QObject *parent = nullptr) : QObject(parent) {}

    // Getter/Setter适配QString类型
    QString orderId() const { return m_orderId; }
    void setOrderId(const QString &orderId) { m_orderId = orderId; }

    int userId() const { return m_userId; }
    void setUserId(int userId) { m_userId = userId; }

    int flightId() const { return m_flightId; }
    void setFlightId(int flightId) { m_flightId = flightId; }

    double totalPrice() const { return m_totalPrice; }
    void setTotalPrice(double totalPrice) { m_totalPrice = totalPrice; }

    QString status() const { return m_status; }
    void setStatus(const QString &status) { m_status = status; }

    QDateTime createTime() const { return m_createTime; }
    void setCreateTime(const QDateTime &createTime) { m_createTime = createTime; }

private:
    QString m_orderId;    // 订单ID（ORD202512010001格式）
    int m_userId = 0;
    int m_flightId = 0;
    double m_totalPrice = 0.0;
    QString m_status;
    QDateTime m_createTime;
};

#endif // ORDERINFO_H
