#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QDateTime>

class User : public QObject {
    Q_OBJECT
    Q_PROPERTY(int userId READ userId WRITE setUserId)
    Q_PROPERTY(QString telephone READ telephone WRITE setTelephone)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QDateTime registerTime READ registerTime WRITE setRegisterTime)

public:
    explicit User(QObject *parent = nullptr) : QObject(parent), m_userId(0) {}

    // Getter
    int userId() const { return m_userId; }
    QString telephone() const { return m_telephone; }
    QString password() const { return m_password; }
    QString username() const { return m_username; }
    QDateTime registerTime() const { return m_registerTime; }

    // Setter
    void setUserId(int userId) { m_userId = userId; }
    void setTelephone(const QString &telephone) { m_telephone = telephone; }
    void setPassword(const QString &password) { m_password = password; }
    void setUsername(const QString &username) { m_username = username; }
    void setRegisterTime(const QDateTime &registerTime) { m_registerTime = registerTime; }

private:
    int m_userId;
    QString m_telephone;
    QString m_password;
    QString m_username;
    QDateTime m_registerTime;
};

#endif // USER_H
