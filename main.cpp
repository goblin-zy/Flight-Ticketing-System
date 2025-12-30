#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>

// 引入HTTP服务器头文件
#include "HttpServer.h"
// 引入数据库管理类
#include "DBManager.h"

int main(int argc, char *argv[])
{
    // 1. 初始化Qt核心应用程序
    QCoreApplication a(argc, argv);

    // 2. 初始化数据库连接（前置操作）
    qDebug() << "===== 初始化数据库连接 =====";
    DBManager& dbManager = DBManager::getInstance();
    if (!dbManager.getConnection().isOpen()) {
        qCritical() << "数据库连接失败：" << dbManager.getConnection().lastError().text();
        return -1;
    }
    qDebug() << "数据库连接成功！";


    // 4. 创建并启动HTTP服务器
    qDebug() << "\n===== 启动HTTP服务器 =====";
    HttpServer server;
    quint16 port = 8080;

    if (server.startServer(port)) {
        qDebug() << QString("HTTP服务器启动成功，监听端口：%1").arg(port);
        qDebug() << "测试地址（本机）：http://localhost:" << port;
        qDebug() << "测试地址（局域网）：http://172.17.39.75:" << port;
        qDebug() << "按 Ctrl+C 停止服务器...";
    } else {
        qCritical() << "HTTP服务器启动失败！";
        return -1;
    }

    // 5. 运行Qt事件循环
    int exitCode = a.exec();

    // 6. 清理资源
    qDebug() << "\n===== 停止服务器 =====";
    dbManager.getConnection().close();
    qDebug() << "服务器已停止，数据库连接已关闭";

    return exitCode;
}
