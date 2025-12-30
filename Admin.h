#ifndef ADMIN_H
#define ADMIN_H

#include <QObject>
#include <QString>

class Admin : public QObject {
    Q_OBJECT
    Q_PROPERTY(int adminId READ adminId WRITE setAdminId)
    Q_PROPERTY(QString adminAccount READ adminAccount WRITE setAdminAccount)
    Q_PROPERTY(QString adminPassword READ adminPassword WRITE setAdminPassword)
    Q_PROPERTY(QString adminName READ adminName WRITE setAdminName)

public:
    explicit Admin(QObject *parent = nullptr)
        : QObject(parent), m_adminId(0) {}

    // --- Getter ---
    int adminId() const { return m_adminId; }
    QString adminAccount() const { return m_adminAccount; }
    QString adminPassword() const { return m_adminPassword; }
    QString adminName() const { return m_adminName; }

    // --- Setter ---
    void setAdminId(int adminId) { m_adminId = adminId; }
    void setAdminAccount(const QString &adminAccount) { m_adminAccount = adminAccount; }
    void setAdminPassword(const QString &adminPassword) { m_adminPassword = adminPassword; }
    void setAdminName(const QString &adminName) { m_adminName = adminName; }

private:
    int m_adminId;              // 管理员主键ID
    QString m_adminAccount;     // 登录账号
    QString m_adminPassword;    // 登录密码
    QString m_adminName;        // 管理员姓名/昵称
};

#endif // ADMIN_H
