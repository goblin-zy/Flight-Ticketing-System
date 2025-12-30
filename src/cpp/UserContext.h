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
    //Q_PROPERTY(QString myAvatar READ myAvatar WRITE setMyAvatar NOTIFY myAvatarChanged)

    // 原有属性保持不变
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString myToken READ myToken WRITE setMyToken NOTIFY myTokenChanged)
    Q_PROPERTY(int myMoney READ myMoney WRITE setMyMoney NOTIFY myMoneyChanged)
    Q_PROPERTY(QString myTelephone READ myTelephone WRITE setMyTelephone NOTIFY myTelephoneChanged)
    
    // 新增管理员相关属性
    Q_PROPERTY(bool isAdmin READ isAdmin WRITE setIsAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(QString adminAccount READ adminAccount WRITE setAdminAccount NOTIFY adminAccountChanged)

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
    
    // 新增管理员相关getter
    bool isAdmin() const;
    QString adminAccount() const;

    // --- Setters ---
    void setUserId(const QString &userId);
    void setUserName(const QString &userName);
    void setMyToken(const QString &myToken);
    void setMyMoney(int myMoney);
    void setMyTelephone(const QString &myTelephone);
    //void setMyAvatar(const QString &avatar); // 新增
    
    // 新增管理员相关setter
    void setIsAdmin(bool isAdmin);
    void setAdminAccount(const QString &adminAccount);

    // --- 功能函数 ---
    // QML 中直接调用此函数即可刷新用户信息
    Q_INVOKABLE void updateUserInfo();

signals:
    void userIdChanged();
    void userNameChanged();
    void myTokenChanged();
    void myMoneyChanged();
    void myTelephoneChanged();
    //void myAvatarChanged(); // 新增
    
    // 新增管理员相关信号
    void isAdminChanged();
    void adminAccountChanged();
    
    // 信号：通知UI用户信息已全部更新完毕，可以安全跳转
    void userInfoReady();

private slots:
    // 处理网络请求成功的槽函数
    void onApiSuccess(const QJsonObject &response);
    // 处理网络请求失败的槽函数
    void onApiFailed(const QString &errorMessage);

private:
    NetworkManager* m_manager; // 持有网络管理器指针

    QString m_userId;
    QString m_userName;
    QString m_myToken;
    int m_myMoney;
    QString m_myTelephone;
   // QString m_myAvatar; // 新增
    
    // 新增管理员相关私有变量
    bool m_isAdmin;
    QString m_adminAccount;
};

#endif // USERCONTEXT_H
