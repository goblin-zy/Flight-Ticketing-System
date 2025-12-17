#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// 单例类：整个程序只维护一个数据库连接
class DBManager {
public:
    // 获取唯一实例（单例核心）
    static DBManager& getInstance() {
        static DBManager instance; // 静态局部变量，保证只创建一次
        return instance;
    }

    // 获取可用的数据库连接
    QSqlDatabase getConnection() {
        return m_db;
    }

private:
    // 私有构造函数（禁止外部创建对象）
    DBManager() {
        // 初始化ODBC连接（替换成你的MySQL信息）
        m_db = QSqlDatabase::addDatabase("QODBC");
        m_db.setHostName("127.0.0.1");    // 数据库地址（本机）
        m_db.setPort(3306);               // MySQL默认端口
        m_db.setDatabaseName(""); // 你的数据库名
        m_db.setUserName("root");         // 你的MySQL用户名
        m_db.setPassword(""); // 替换成实际密码

        // 测试连接
        if (!m_db.open()) {
            qDebug() << "数据库连接失败：" << m_db.lastError().text();
        } else {
            qDebug() << "数据库连接成功！";
        }
    }

    // 禁止拷贝和赋值（单例类必须）
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

    QSqlDatabase m_db; // 数据库连接对象
};

#endif // DBMANAGER_H
