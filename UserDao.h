#ifndef USERDAO_H
#define USERDAO_H

#include "DBManager.h"
#include "User.h"
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDateTime>
#include <QSqlError>
#include "TokenTool.h"

// UserDao.h 修改后的完整版本：

class UserDao {
public:
    // 1. 获取用户完整信息
    QJsonObject getUserInfoByToken(const QString& authorizationHeader) {
        QJsonObject response;
        int userId = TokenTool::verifyToken(authorizationHeader);

        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }

        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT user_id, telephone, username, balance, register_time FROM user WHERE user_id = ?");
        query.addBindValue(userId);

        if (query.exec() && query.next()) {
            QJsonObject data;
            data["userId"] = query.value("user_id").toInt();
            data["telephone"] = query.value("telephone").toString();
            data["username"] = query.value("username").toString();

            // 数据库存的是分，直接返回分
            data["balance"] = query.value("balance").toInt();

            data["registerTime"] = query.value("register_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");

            response["code"] = 200;
            response["message"] = "获取成功";
            response["data"] = data;
        } else {
            response["code"] = 500;
            response["message"] = "用户不存在";
            response["data"] = QJsonValue::Null;
        }
        return response;
    }

    // 2. 根据手机号查询用户
    User* getUserByTelephone(const QString& telephone) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT * FROM user WHERE telephone = ?");
        query.addBindValue(telephone);
        if (query.exec() && query.next()) {
            User* user = new User();
            user->setUserId(query.value("user_id").toInt());
            user->setTelephone(query.value("telephone").toString());
            user->setPassword(query.value("password").toString());
            user->setUsername(query.value("username").toString());
            user->setRegisterTime(query.value("register_time").toDateTime());

            // 数据库存的是分，直接设置
            user->setBalance(query.value("balance").toInt());

            return user;
        }
        return nullptr;
    }

    // 3. 登录业务
    QJsonObject login(const QString& telephone, const QString& password) {
        QJsonObject response;
        User* user = getUserByTelephone(telephone);

        if (!user || user->password() != password) {
            if(user) delete user;
            response["code"] = 500;
            response["message"] = "账号或密码错误";
            response["data"] = QJsonValue::Null;
            return response;
        }

        QString token = TokenTool::generateToken(user->userId());
        TokenTool::saveToken(user->userId(), token);

        QJsonObject data;
        data["userId"] = user->userId();
        data["username"] = user->username();
        data["telephone"] = user->telephone();
        // 数据库存的是分，直接返回
        data["balance"] = user->balance();
        data["token"] = token;

        response["code"] = 200;
        response["message"] = "登录成功";
        response["data"] = data;

        delete user;
        return response;
    }

    // 4. 注册业务
    QJsonObject registerUser(const QString& telephone, const QString& password, const QString& username) {
        QJsonObject response;
        QSqlQuery query(DBManager::getInstance().getConnection());

        if (getUserByTelephone(telephone)) {
            response["code"] = 500;
            response["message"] = "手机号已被注册";
            response["data"] = QJsonValue::Null;
            return response;
        }

        query.prepare("INSERT INTO user (telephone, password, username, register_time, balance) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(telephone);
        query.addBindValue(password);
        query.addBindValue(username);
        query.addBindValue(QDateTime::currentDateTime());
        query.addBindValue(0); // 初始余额 0 分

        if (query.exec()) {
            QJsonObject data;
            data["userId"] = query.lastInsertId().toInt();
            data["telephone"] = telephone;
            data["username"] = username;
            response["code"] = 200;
            response["message"] = "注册成功";
            response["data"] = data;
        } else {
            response["code"] = 500;
            response["message"] = "注册失败";
        }
        return response;
    }

    // 5. 修改用户信息 API
    QJsonObject updateUserInfoApi(const QString& authorizationHeader, const QString& newUsername,
                                  const QString& oldPassword, const QString& newPassword, int addBalanceFen = -1) {
        QJsonObject response;
        int userId = TokenTool::verifyToken(authorizationHeader);

        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }

        QString sql = "UPDATE user SET ";
        QList<QVariant> bindValues;
        bool hasUpdates = false;

        // 处理用户名
        if (!newUsername.isEmpty()) {
            sql += "username = ?";
            bindValues << newUsername;
            hasUpdates = true;
        }

        // 处理密码 (需验证旧密码)
        if (!newPassword.isEmpty()) {
            QSqlQuery checkQuery(DBManager::getInstance().getConnection());
            checkQuery.prepare("SELECT password FROM user WHERE user_id = ?");
            checkQuery.addBindValue(userId);
            if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toString() != oldPassword) {
                response["code"] = 500;
                response["message"] = "旧密码验证失败";
                return response;
            }
            if (hasUpdates) sql += ", ";
            sql += "password = ?";
            bindValues << newPassword;
            hasUpdates = true;
        }

        // 处理余额充值: 数据库存的是分，直接加
        if (addBalanceFen >= 0) {
            if (hasUpdates) sql += ", ";
            sql += "balance = balance + ?";
            bindValues << addBalanceFen; // 直接加分，不需要转换
            hasUpdates = true;
        }

        if (!hasUpdates) {
            response["code"] = 200;
            response["message"] = "无需要更新的字段";
            return response;
        }

        sql += " WHERE user_id = ?";
        bindValues << userId;

        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare(sql);
        for (const auto& val : bindValues) query.addBindValue(val);

        if (query.exec()) {
            QJsonObject data;
            data["userId"] = userId;

            // 查询最新数据返回前端
            QSqlQuery fetchQuery(DBManager::getInstance().getConnection());
            fetchQuery.prepare("SELECT username, balance FROM user WHERE user_id = ?");
            fetchQuery.addBindValue(userId);

            if (fetchQuery.exec() && fetchQuery.next()) {
                data["username"] = fetchQuery.value("username").toString();
                // 数据库存的是分，直接返回
                data["balance"] = fetchQuery.value("balance").toInt();
            }

            response["code"] = 200;
            response["message"] = "修改成功";
            response["data"] = data;
        } else {
            response["code"] = 500;
            response["message"] = "修改失败: " + query.lastError().text();
        }

        return response;
    }

    // 6. 添加 addUser 方法（供 UserService 使用）
    bool addUser(User* user, int& newUserId) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("INSERT INTO user (telephone, password, username, register_time, balance) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(user->telephone());
        query.addBindValue(user->password());
        query.addBindValue(user->username());
        query.addBindValue(QDateTime::currentDateTime());
        query.addBindValue(user->balance()); // 已经是分

        if (query.exec()) {
            newUserId = query.lastInsertId().toInt();
            return true;
        }
        return false;
    }

    // 7. 添加 updateUserInfo 方法（供 UserService 使用）
    bool updateUserInfo(int userId, const QString& newUsername,
                        const QString& oldPassword, const QString& newPassword) {
        QSqlQuery query(DBManager::getInstance().getConnection());

        // 验证旧密码
        query.prepare("SELECT password FROM user WHERE user_id = ?");
        query.addBindValue(userId);
        if (!query.exec() || !query.next() || query.value(0).toString() != oldPassword) {
            return false;
        }

        // 更新用户名和密码
        query.prepare("UPDATE user SET username = ?, password = ? WHERE user_id = ?");
        query.addBindValue(newUsername);
        query.addBindValue(newPassword);
        query.addBindValue(userId);

        return query.exec();
    }
};

#endif // USERDAO_H
