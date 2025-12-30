#ifndef ORDERINFO_H
#define ORDERINFO_H
#include <QObject>
#include <QDateTime>
#include <QString>

class OrderInfo : public QObject
{
    Q_OBJECT
    // 严格对应order_info表字段 + API返回字段
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)       // 订单ID（PK，ORD格式）
    Q_PROPERTY(int userId READ userId WRITE setUserId)             // 用户ID（外键）
    Q_PROPERTY(int flightId READ flightId WRITE setFlightId)       // 航班ID（外键）
    Q_PROPERTY(int totalPrice READ totalPrice WRITE setTotalPrice) // 总金额（分，整数）
    Q_PROPERTY(QString status READ status WRITE setStatus)         // 状态：待支付/已支付/已退签等
    Q_PROPERTY(QDateTime createTime READ createTime WRITE setCreateTime) // 创建时间
    Q_PROPERTY(QDateTime payTime READ payTime WRITE setPayTime)     // 支付时间（ nullable）
    Q_PROPERTY(QDateTime refundTime READ refundTime WRITE setRefundTime) // 退签时间（nullable）

public:
    explicit OrderInfo(QObject *parent = nullptr) : QObject(parent) {}

    // Getter（严格对应属性名）
    QString orderId() const { return m_orderId; }
    int userId() const { return m_userId; }
    int flightId() const { return m_flightId; }
    int totalPrice() const { return m_totalPrice; }
    QString status() const { return m_status; }
    QDateTime createTime() const { return m_createTime; }
    QDateTime payTime() const { return m_payTime; }
    QDateTime refundTime() const { return m_refundTime; }

    // Setter
    void setOrderId(const QString &orderId) { m_orderId = orderId; }
    void setUserId(int userId) { m_userId = userId; }
    void setFlightId(int flightId) { m_flightId = flightId; }
    void setTotalPrice(int totalPrice) { m_totalPrice = totalPrice; }
    void setStatus(const QString &status) { m_status = status; }
    void setCreateTime(const QDateTime &createTime) { m_createTime = createTime; }
    void setPayTime(const QDateTime &payTime) { m_payTime = payTime; }
    void setRefundTime(const QDateTime &refundTime) { m_refundTime = refundTime; }

private:
    QString m_orderId;          // 订单ID（如ORD202512010001）
    int m_userId = 0;           // 关联用户ID
    int m_flightId = 0;         // 关联航班ID
    int m_totalPrice = 0;       // 总金额（分）
    QString m_status;          // 订单状态
    QDateTime m_createTime;     // 创建时间
    QDateTime m_payTime;       // 支付时间（未支付时为空）
    QDateTime m_refundTime;     // 退签时间（未退签时为空）
};

#endif // ORDERINFO_H
