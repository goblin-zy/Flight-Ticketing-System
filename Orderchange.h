#ifndef ORDERCHANGE_H
#define ORDERCHANGE_H

#include <QObject>
#include <QString>
#include <QDateTime>

class OrderChange : public QObject {
    Q_OBJECT
    Q_PROPERTY(int changeId READ changeId WRITE setChangeId)
    Q_PROPERTY(QString orderId READ orderId WRITE setOrderId)
    Q_PROPERTY(int oldFlightId READ oldFlightId WRITE setOldFlightId)
    Q_PROPERTY(int newFlightId READ newFlightId WRITE setNewFlightId)
    Q_PROPERTY(int priceDifference READ priceDifference WRITE setPriceDifference)
    Q_PROPERTY(QDateTime changeTime READ changeTime WRITE setChangeTime)

public:
    explicit OrderChange(QObject *parent = nullptr) : QObject(parent), m_changeId(0), m_oldFlightId(0), m_newFlightId(0), m_priceDifference(0) {}

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
    int m_changeId;
    QString m_orderId;
    int m_oldFlightId;
    int m_newFlightId;
    int m_priceDifference;
    QDateTime m_changeTime;
};

#endif // ORDERCHANGE_H
