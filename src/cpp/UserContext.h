#ifndef USERCONTEXT_H
#define USERCONTEXT_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QDebug>
#include "NetworkManager.h" // 必须包含，以便调用 request

class UserContext : public QObject
{
    Q_OBJECT
    // 1. 新增 Avatar 属性
    Q_PROPERTY(QString myAvatar READ myAvatar WRITE setMyAvatar NOTIFY myAvatarChanged)

    // 原有属性保持不变
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString myToken READ myToken WRITE setMyToken NOTIFY myTokenChanged)
    Q_PROPERTY(int myMoney READ myMoney WRITE setMyMoney NOTIFY myMoneyChanged)
    Q_PROPERTY(QString myTelephone READ myTelephone WRITE setMyTelephone NOTIFY myTelephoneChanged)

public:
    // 修改构造函数，强制要求传入 NetworkManager 指针
    explicit UserContext(NetworkManager* manager, QObject *parent = nullptr);

    // --- Getters ---
    QString userId() const;
    QString userName() const;
    QString myToken() const;
    int myMoney() const;
    QString myTelephone() const;
    QString myAvatar() const; // 新增

    // --- Setters ---
    void setUserId(const QString &userId);
    void setUserName(const QString &userName);
    void setMyToken(const QString &myToken);
    void setMyMoney(int myMoney);
    void setMyTelephone(const QString &myTelephone);
    void setMyAvatar(const QString &avatar); // 新增

    // --- 功能函数 ---
    // QML 中直接调用此函数即可刷新用户信息
    Q_INVOKABLE void updateUserInfo();

signals:
    void userIdChanged();
    void userNameChanged();
    void myTokenChanged();
    void myMoneyChanged();
    void myTelephoneChanged();
    void myAvatarChanged(); // 新增

private slots:
    // 处理网络请求成功的槽函数
    void onApiSuccess(const QJsonObject &response);

private:
    NetworkManager* m_manager; // 持有网络管理器指针

    QString m_userId;
    QString m_userName;
    QString m_myToken;
    int m_myMoney;
    QString m_myTelephone;
    QString m_myAvatar; // 新增
};

#endif // USERCONTEXT_H
