//userservice用于协调多个 UserDao 方法完成复杂业务

#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "UserDao.h"
#include "TokenTool.h"
#include <QVariantMap>

class UserService {
public:

    // 新增：获取用户信息接口（适配1.4.1 API）
    QJsonObject getUserInfo(const QString& token) {
        UserDao dao;
        return dao.getUserInfoByToken(token);
    }

    // 登录接口（返回userId、username、token）
    QVariantMap login(const QString& telephone, const QString& password) {
        QVariantMap result;
        UserDao dao;
        User* user = dao.getUserByTelephone(telephone);

        if (!user) {
            result["success"] = false;
            result["message"] = "用户不存在";
            return result;
        }

        if (user->password() != password) {
            result["success"] = false;
            result["message"] = "密码错误";
            delete user;
            return result;
        }

        // 生成token
        QString token = TokenTool::generateToken(user->userId());
        TokenTool::saveToken(user->userId(), token);
        result["success"] = true;
        result["data"] = QVariantMap{
            {"userId", user->userId()},
            {"username", user->username()},
            {"token", token}
        };

        delete user;
        return result;
    }

    // 注册接口（返回userId）
    QVariantMap registerUser(const QString& telephone, const QString& password,
                             const QString& username) {
        QVariantMap result;
        UserDao dao;

        // 直接调用UserDao中的registerUser方法
        QJsonObject jsonResult = dao.registerUser(telephone, password, username);

        if (jsonResult["code"].toInt() == 200) {
            result["success"] = true;
            result["data"] = QVariantMap{{"userId", jsonResult["data"].toObject()["userId"].toInt()}};
        } else {
            result["success"] = false;
            result["message"] = jsonResult["message"].toString();
        }
        return result;
    }

    // 个人信息修改接口
    QVariantMap updateUser(int userId, const QString& newUsername, const QString& oldPassword, const QString& newPassword) {
        QVariantMap result;
        UserDao dao;

        if (dao.updateUserInfo(userId, newUsername, oldPassword, newPassword)) {
            result["success"] = true;
            result["message"] = "修改成功";
        } else {
            result["success"] = false;
            result["message"] = "修改失败（旧密码错误或用户不存在）";
        }
        return result;
    }

    // 新增：通过Token修改用户信息（适配API的token验证逻辑）
    QJsonObject updateUser(const QString& token, const QString& newUsername,
                           const QString& oldPassword, const QString& newPassword,
                           int newBalance = -1) {
        UserDao dao;
        // 直接传递完整的token（包含Bearer前缀），让UserDao内部处理
        return dao.updateUserInfoApi(token, newUsername, oldPassword, newPassword, newBalance);
    }

};

#endif // USERSERVICE_H
