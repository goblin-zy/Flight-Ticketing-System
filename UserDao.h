#ifndef USERDAO_H
#define USERDAO_H

#include "DBManager.h"
#include "User.h"
#include <QList>

class UserDao {
public:
    // 1. 根据手机号查询用户（登录/验证用）
    User* getUserByTelephone(const QString& telephone) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        // 预处理SQL（防注入）
        query.prepare("SELECT * FROM user WHERE telephone = ?");
        query.addBindValue(telephone);

        if (query.exec() && query.next()) {
            // 创建User对象，映射数据库字段
            User* user = new User();
            user->setUserId(query.value("user_id").toInt());
            user->setTelephone(query.value("telephone").toString());
            user->setPassword(query.value("password").toString());
            user->setUsername(query.value("username").toString());
            user->setRegisterTime(query.value("register_time").toDateTime());
            return user;
        }
        return nullptr; // 未查到返回空
    }

    // 2. 新增用户（注册用）
    bool addUser(User* user) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("INSERT INTO user (telephone, password, username, register_time) VALUES (?, ?, ?, ?)");
        query.addBindValue(user->telephone());
        query.addBindValue(user->password());
        query.addBindValue(user->username());
        query.addBindValue(user->registerTime());

        return query.exec(); // 执行成功返回true
    }

    // 更多操作（如修改密码、删除用户）可后续补充
};

#endif // USERDAO_H
