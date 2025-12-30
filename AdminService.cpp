#include "AdminService.h"
#include "AdminDao.h"
#include "TokenTool.h"
#include "DBManager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>       // 必须添加，否则 query.lastError() 报错
#include <QNetworkRequest> // 必须添加，否则 getAdminInfo 参数类型不识别
#include <QDateTime>
#include <QNetworkRequest>

// 管理员登录
QJsonObject AdminService::login(const QString& account, const QString& pwd) {
    AdminDao dao;
    QJsonObject adminData = dao.login(account, pwd);

    if (!adminData.isEmpty()) {

        int adminId = adminData["adminId"].toInt();
        QString token = TokenTool::generateToken(adminId);
        TokenTool::saveToken(adminId, token);

        QJsonObject data;
        data["adminId"] = adminId;
        data["adminAccount"] = account;
        data["adminName"] = adminData["adminName"].toString();
        data["token"] = "Bearer " + token;

        return {{"code", 200}, {"message", "登录成功"}, {"data", data}};
    }
    return {{"code", 500}, {"message", "账号或密码错误"}};
}

QJsonObject AdminService::getAdminInfo(const QNetworkRequest& request) {
    // 1. 从请求头获取Authorization字段
    QByteArray authHeader = request.rawHeader("Authorization");
    if (authHeader.isEmpty()) {
        return {{"code", 401}, {"message", "未提供Authorization头"}};
    }

    // 2. 直接传入完整的Authorization头，TokenTool内部会处理Bearer前缀
    QString authorizationHeader = QString::fromUtf8(authHeader);
    // 先去掉 "Bearer " 前缀，再校验
    QString pureToken = TokenTool::removeBearerPrefix(authorizationHeader);
    int adminId = TokenTool::verifyToken(pureToken);

    if (adminId == -1) {
        return {{"code", 401}, {"message", "token无效或已过期"}};
    }

    // 3. 查询管理员信息
    QSqlQuery query(DBManager::getInstance().getConnection());
    query.prepare("SELECT admin_account, admin_name FROM admin WHERE admin_id = ?");
    query.addBindValue(adminId);

    if (query.exec() && query.next()) {
        QJsonObject data;
        data["adminId"] = adminId;
        data["adminAccount"] = query.value("admin_account").toString();
        data["adminName"] = query.value("admin_name").toString();
        return {{"code", 200}, {"message", "success"}, {"data", data}};
    } else {
        qDebug() << "获取管理员信息失败：" << query.lastError().text();
        return {{"code", 500}, {"message", "服务器内部错误：无法获取管理员信息"}};
    }
}

// 添加航班
QJsonObject AdminService::addFlight(const QJsonObject& flightJson) {
    AdminDao dao;
    int newFlightId = 0;
    if (dao.addFlight(flightJson, newFlightId)) {
        QJsonObject data;
        data["flightId"] = newFlightId;
        return {{"code", 200}, {"message", "航班添加成功"}, {"data", data}};
    }
    return {{"code", 500}, {"message", "添加失败，请检查数据格式"}};
}

// 删除航班
QJsonObject AdminService::deleteFlight(int flightId) {
    AdminDao dao;
    QString errorMsg;
    if (dao.deleteFlight(flightId, errorMsg)) {
        return {{"code", 200}, {"message", "删除成功"}};
    }
    return {{"code", 500}, {"message", errorMsg.isEmpty() ? "删除失败" : errorMsg}};
}

// 修改航班
QJsonObject AdminService::updateFlight(const QJsonObject& flightJson) {
    AdminDao dao;
    if (dao.updateFlight(flightJson)) {
        return {{"code", 200}, {"message", "更新成功"}};
    }
    return {{"code", 500}, {"message", "更新失败"}};
}

/**
 * @brief [2.3.2] 分页查询客户列表
 * 逻辑：支持 userId/手机/用户名 筛选，并进行字段脱敏（去除余额）
 */
QJsonObject AdminService::queryAllUsers(const QJsonObject& params) {
    AdminDao dao;

    // 1. 提取过滤参数 (优化提取逻辑)
    // 使用 toVariant().toInt() 是最安全的方法，能同时兼容 "1" (字符串) 和 1 (数字)
    int userId = 0;
    if (params.contains("userId")) {
        userId = params.value("userId").toVariant().toInt();
    }

    QString tel = params["telephone"].toString();
    QString name = params["username"].toString();

    // 2. 提取分页参数并设置默认值
    // 同样建议对分页参数使用 toVariant() 增强兼容性
    int pageNum = params.value("pageNum").toVariant().toInt();
    int pageSize = params.value("pageSize").toVariant().toInt();
    if (pageNum <= 0) pageNum = 1;
    if (pageSize <= 0) pageSize = 20;

    // 3. 获取原始数据
    QJsonObject rawData = dao.queryUsersPaged(tel, name, pageNum, pageSize, userId);

    // 4. --- 结构重组 (适配目标 2.3.2 规范) ---
    QJsonObject data;
    data.insert("total", rawData["total"].toInt());
    data.insert("pageNum", pageNum);
    data.insert("pageSize", pageSize);

    QJsonArray cleanList;
    QJsonArray rawList = rawData["list"].toArray();
    for (const QJsonValue& value : rawList) {
        QJsonObject userObj = value.toObject();
        QJsonObject cleanUser;
        // 严格按照文档要求只提取四个字段，剔除敏感的 balance
        cleanUser.insert("userId", userObj["userId"]);
        cleanUser.insert("telephone", userObj["telephone"]);
        cleanUser.insert("username", userObj["username"]);
        cleanUser.insert("registerTime", userObj["registerTime"]);
        cleanList.append(cleanUser);
    }
    data.insert("list", cleanList);

    // 5. 按照规范，如果没有匹配数据则返回 500
    if (data["total"].toInt() == 0) {
        return {{"code", 500}, {"message", "无数据"}, {"data", QJsonValue::Null}};
    }

    return {{"code", 200}, {"message", "查询成功"}, {"data", data}};
}

// 查询所有订单
        QJsonObject AdminService::queryAllOrders(const QJsonObject& params) {
    AdminDao dao;

    // 设置默认分页参数
    QJsonObject queryParams = params;
    if (!queryParams.contains("pageNum")) queryParams["pageNum"] = 1;
    if (!queryParams.contains("pageSize")) queryParams["pageSize"] = 10;

    // 调用DAO层方法
    QJsonObject data = dao.queryOrdersAdmin(queryParams);
    return {{"code", 200}, {"message", "success"}, {"data", data}};
}
