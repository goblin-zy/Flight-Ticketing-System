#ifndef ADMINSERVICE_H
#define ADMINSERVICE_H

#include <QJsonObject>
#include <QString>
#include <QNetworkRequest>  // 添加这行
#include "TokenTool.h"
#include "DBManager.h"

class AdminService {
public:
    // [2.1.1] 管理员登录
    static QJsonObject login(const QString& account, const QString& pwd);

    // [API 附录] 获取管理员信息 (用于前端 UserContext 初始化)
    static QJsonObject getAdminInfo(const QNetworkRequest& request);

    // [2.2.1] 添加航班
    static QJsonObject addFlight(const QJsonObject& flightJson);

    // [2.2.2] 删除航班
    static QJsonObject deleteFlight(int flightId);

    // [2.2.3] 修改航班
    static QJsonObject updateFlight(const QJsonObject& flightJson);

    // [2.3.2] 查询所有客户 (分页)
    static QJsonObject queryAllUsers(const QJsonObject& params);

    // [2.3.3] 查询所有订单 (分页/多条件)
    static QJsonObject queryAllOrders(const QJsonObject& params);
};

#endif // ADMINSERVICE_H
