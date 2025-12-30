#ifndef ORDERCHANGE_H
#define ORDERCHANGE_H
#include <QObject>
#include <QString>
#include <QDateTime>

class OrderChange : public QObject
{
    Q_OBJECT
    // 严格对应order_change表字段 + API改签返回字段
    Q_PROPERTY(int changeId READ changeId WRITE setChangeId)             // 改签记录ID（PK）
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)           // 关联订单ID（外键）
    Q_PROPERTY(int oldFlightId READ oldFlightId WRITE setOldFlightId)   // 原航班ID
    Q_PROPERTY(int newFlightId READ newFlightId WRITE setNewFlightId)   // 新航班ID
    Q_PROPERTY(int priceDifference READ priceDifference WRITE setPriceDifference) // 差价（分）
    Q_PROPERTY(QDateTime changeTime READ changeTime WRITE setChangeTime) // 改签时间

public:
    explicit OrderChange(QObject *parent = nullptr) : QObject(parent),
        m_changeId(0), m_oldFlightId(0), m_newFlightId(0), m_priceDifference(0) {}

    // Getter
    int changeId() const { return m_changeId; }
    QString orderId() const { return m_orderId; }
    int oldFlightId() const { return m_oldFlightId; }
    int newFlightId() const { return m_newFlightId; }
    int priceDifference() const { return m_priceDifference; }
    QDateTime changeTime() const { return m_changeTime; }

    // Setter
    void setChangeId(int changeId) { m_changeId = changeId; }
    void setOrderId(const QString &orderId) { m_orderId = orderId; }
    void setOldFlightId(int oldFlightId) { m_oldFlightId = oldFlightId; }
    void setNewFlightId(int newFlightId) { m_newFlightId = newFlightId; }
    void setPriceDifference(int priceDifference) { m_priceDifference = priceDifference; }
    void setChangeTime(const QDateTime &changeTime) { m_changeTime = changeTime; }

private:
    int m_changeId;             // 改签记录自增ID
    QString m_orderId;          // 关联订单ID
    int m_oldFlightId;          // 原航班ID
    int m_newFlightId;          // 新航班ID
    int m_priceDifference;      // 差价（分：正数补款，负数退款）
    QDateTime m_changeTime;     // 改签时间
};

#endif // ORDERCHANGE_H
