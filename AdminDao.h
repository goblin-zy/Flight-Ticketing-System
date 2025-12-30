#ifndef ADMINDAO_H
#define ADMINDAO_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include "DBManager.h"

class AdminDao {
public:
    AdminDao() = default;

    // [2.1.1] 管理员登录验证
    QJsonObject login(const QString& account, const QString& pwd) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::login"; // 函数进入监控
        qDebug() << "[DAO-DEBUG] 管理员登录尝试, 账号:" << account;
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT admin_id, admin_name FROM admin WHERE admin_account = ? AND admin_password = ?");
        query.addBindValue(account);
        query.addBindValue(pwd);

        if (query.exec()) {
            if (query.next()) {
                QJsonObject obj;
                obj["adminId"] = query.value("admin_id").toInt();
                obj["adminName"] = query.value("admin_name").toString();
                qDebug() << "[DAO-DEBUG] 登录成功, AdminID:" << obj["adminId"].toInt();
                return obj;
            } else {
                qDebug() << "[DAO-DEBUG] 登录失败: 账号或密码错误";
            }
        } else {
            qDebug() << "[DAO-DEBUG] SQL执行错误:" << query.lastError().text();
        }
        return QJsonObject();
    }

    // [2.2.1] 添加航班
    bool addFlight(const QJsonObject& json, int& outId) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::addFlight"; // 函数进入监控
        qDebug() << "[DAO-DEBUG] 开始添加航班, 航班号:" << json["flightNumber"].toString();
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("INSERT INTO flight (flight_number, departure_city, arrival_city, departure_airport, "
                      "arrival_airport, date, departure_time, arrival_time, checkin_start_time, "
                      "checkin_end_time, price, airline_company, status, left_tickets) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        query.addBindValue(json["flightNumber"].toString());
        query.addBindValue(json["departureCity"].toString());
        query.addBindValue(json["arrivalCity"].toString());
        query.addBindValue(json["departureAirport"].toString());
        query.addBindValue(json["arrivalAirport"].toString());
        query.addBindValue(json["date"].toString());
        query.addBindValue(json["departureTime"].toString());
        query.addBindValue(json["arrivalTime"].toString());
        query.addBindValue(json["checkinStartTime"].toString());
        query.addBindValue(json["checkinEndTime"].toString());
        query.addBindValue(json["price"].toInt());
        query.addBindValue(json["airlineCompany"].toString());
        query.addBindValue(json["status"].toString());
        query.addBindValue(json["leftTickets"].toInt());

        if (query.exec()) {
            outId = query.lastInsertId().toInt();
            qDebug() << "[DAO-DEBUG] 航班添加成功, 新分配ID:" << outId;
            return true;
        }
        qDebug() << "[DAO-DEBUG] 添加航班失败:" << query.lastError().text();
        return false;
    }

    // [2.2.2] 删除航班 (需检查是否有订单关联)
    bool deleteFlight(int flightId, QString& errorMsg) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::deleteFlight"; // 函数进入监控
        qDebug() << "[DAO-DEBUG] 尝试删除航班ID:" << flightId;
        QSqlDatabase db = DBManager::getInstance().getConnection();
        QSqlQuery query(db);

        // 1. 检查是否有未取消的订单关联
        query.prepare("SELECT COUNT(*) FROM order_info WHERE flight_id = ? AND status != '已取消'");
        query.addBindValue(flightId);
        if (query.exec() && query.next()) {
            int count = query.value(0).toInt();
            if (count > 0) {
                errorMsg = "该航班尚有有效订单，无法删除";
                qDebug() << "[DAO-DEBUG] 拦截删除:" << errorMsg << ", 订单数:" << count;
                return false;
            }
        }

        // 2. 执行删除
        query.prepare("DELETE FROM flight WHERE flight_id = ?");
        query.addBindValue(flightId);
        bool ok = query.exec();
        if(!ok) qDebug() << "[DAO-DEBUG] 删除SQL失败:" << query.lastError().text();
        else qDebug() << "[DAO-DEBUG] 删除成功";
        return ok;
    }

    // [2.2.3] 修改航班 (动态SQL实现)
    bool updateFlight(const QJsonObject& json) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::updateFlight"; // 函数进入监控
        int flightId = json["flightId"].toInt();
        qDebug() << "[DAO-DEBUG] 开始更新航班ID:" << flightId;
        if (!json.contains("flightId")) return false;

        QStringList updateFields;
        // 动态检查 JSON 包含哪些字段就更新哪些
        if (json.contains("flightNumber")) updateFields << "flight_number = :num";
        if (json.contains("price")) updateFields << "price = :price";
        if (json.contains("status")) updateFields << "status = :status";
        if (json.contains("leftTickets")) updateFields << "left_tickets = :tickets";
        if (json.contains("departureTime")) updateFields << "departure_time = :dtime";
        if (json.contains("arrivalTime")) updateFields << "arrival_time = :atime";

        if (updateFields.isEmpty()) {
            qDebug() << "[DAO-DEBUG] 无更新字段";
            return true;
        }

        QString sql = "UPDATE flight SET " + updateFields.join(", ") + " WHERE flight_id = :id";
        qDebug() << "[DAO-DEBUG] 生成的SQL:" << sql;

        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare(sql);

        // 绑定参数
        query.bindValue(":id", flightId);
        if (json.contains("flightNumber")) query.bindValue(":num", json["flightNumber"].toString());
        if (json.contains("price")) query.bindValue(":price", json["price"].toInt());
        if (json.contains("status")) query.bindValue(":status", json["status"].toString());
        if (json.contains("leftTickets")) query.bindValue(":tickets", json["leftTickets"].toInt());
        if (json.contains("departureTime")) query.bindValue(":dtime", json["departureTime"].toString());
        if (json.contains("arrivalTime")) query.bindValue(":atime", json["arrivalTime"].toString());

        bool ok = query.exec();
        if(!ok) qDebug() << "[DAO-DEBUG] 更新SQL失败:" << query.lastError().text();
        else qDebug() << "[DAO-DEBUG] 更新成功";
        return ok;
    }

    // [2.3.2] 查询所有客户 (带分页和搜索) - 优化 ID 过滤逻辑
    QJsonObject queryUsersPaged(const QString& telephone, const QString& username, int pageNum, int pageSize, int userId = 0) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::queryUsersPaged";
        qDebug() << "[DAO-DEBUG] 过滤参数 -> ID:" << userId << "手机:" << telephone << "用户名:" << username;

        QSqlQuery query(DBManager::getInstance().getConnection());

        // 1. 动态构造条件列表
        QStringList filters;
        if (userId > 0) {
            filters << QString("user_id = %1").arg(userId);
        }
        if (!telephone.isEmpty()) {
            filters << QString("telephone LIKE '%%1%'").arg(telephone);
        }
        if (!username.isEmpty()) {
            filters << QString("username LIKE '%%1%'").arg(username);
        }

        // 2. 组装 WHERE 子句
        QString whereClause = "";
        if (!filters.isEmpty()) {
            whereClause = " WHERE " + filters.join(" AND ");
        }

        // 3. 获取总数
        int total = 0;
        QString countSql = "SELECT COUNT(*) FROM user" + whereClause;
        if(query.exec(countSql) && query.next()) {
            total = query.value(0).toInt();
        }

        // 4. 执行分页查询
        // 注意：LIMIT 和 OFFSET 之间必须有空格，%1 后面也需要空格
        QString sql = QString("SELECT user_id, telephone, username, balance, register_time FROM user %1 LIMIT %2 OFFSET %3")
                          .arg(whereClause)
                          .arg(pageSize)
                          .arg((pageNum - 1) * pageSize);

        qDebug() << "[DAO-DEBUG] 执行SQL:" << sql;

        QJsonArray arr;
        if (query.exec(sql)) {
            while (query.next()) {
                QJsonObject u;
                u["userId"] = query.value("user_id").toInt();
                u["telephone"] = query.value("telephone").toString();
                u["username"] = query.value("username").toString();
                u["balance"] = query.value("balance").toInt();
                u["registerTime"] = query.value("register_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");
                arr.append(u);
            }
        } else {
            qDebug() << "[DAO-DEBUG] SQL执行失败:" << query.lastError().text();
        }

        QJsonObject res;
        res["total"] = total;
        res["list"] = arr;
        return res;
    }

    // [2.3.3] 查询所有订单 (带分页和多条件搜索) - 增强orderId筛选
    QJsonObject queryOrdersAdmin(const QJsonObject& params) {
        qDebug() << "[DAO-DEBUG] >>> 进入 AdminDao::queryOrdersAdmin";
        qDebug() << "[DAO-DEBUG] 管理员查询订单, 参数:" << params;

        QSqlQuery query(DBManager::getInstance().getConnection());
        QString where = " WHERE 1=1";

        // ===== orderId 精确筛选 =====
        if (params.contains("orderId") && !params["orderId"].toString().isEmpty()) {
            QString orderIdValue = params["orderId"].toString();
            // 使用LIKE进行模糊匹配（如果前端需要精确匹配，可以改为 = 号）
            where += QString(" AND o.order_id LIKE '%%1%'").arg(orderIdValue);
            qDebug() << "[DAO-DEBUG] 添加orderId筛选条件:" << orderIdValue;
        }

        // ===== 电话筛选 =====
        if (params.contains("telephone") && !params["telephone"].toString().isEmpty()) {
            QString telValue = params["telephone"].toString();
            where += QString(" AND u.telephone LIKE '%%1%'").arg(telValue);
            qDebug() << "[DAO-DEBUG] 添加电话筛选条件:" << telValue;
        }

        // ===== 状态筛选 =====
        if (params.contains("status") && !params["status"].toString().isEmpty()) {
            QString statusValue = params["status"].toString();
            where += QString(" AND o.status = '%1'").arg(statusValue);
            qDebug() << "[DAO-DEBUG] 添加状态筛选条件:" << statusValue;
        }

        // ===== 日期范围筛选 =====
        if (params.contains("startDate") && !params["startDate"].toString().isEmpty()) {
            QString startDate = params["startDate"].toString();
            where += QString(" AND DATE(o.create_time) >= '%1'").arg(startDate);
            qDebug() << "[DAO-DEBUG] 添加开始日期筛选:" << startDate;
        }

        if (params.contains("endDate") && !params["endDate"].toString().isEmpty()) {
            QString endDate = params["endDate"].toString();
            where += QString(" AND DATE(o.create_time) <= '%1'").arg(endDate);
            qDebug() << "[DAO-DEBUG] 添加结束日期筛选:" << endDate;
        }

        // ===== 用户ID筛选 =====
        if (params.contains("userId") && params["userId"].toInt() > 0) {
            int userId = params["userId"].toInt();
            where += QString(" AND o.user_id = %1").arg(userId);
            qDebug() << "[DAO-DEBUG] 添加用户ID筛选:" << userId;
        }

        // 联表查用户手机号
        QString sqlBase = "FROM order_info o JOIN user u ON o.user_id = u.user_id " + where;

        // 获取总数
        QString countSql = "SELECT COUNT(*) " + sqlBase;
        qDebug() << "[DAO-DEBUG] 总数SQL:" << countSql;

        int total = 0;
        if(query.exec(countSql) && query.next()) {
            total = query.value(0).toInt();
            qDebug() << "[DAO-DEBUG] 查询到的总数:" << total;
        }

        // 分页参数
        int pageNum = params["pageNum"].toInt(1);
        int pageSize = params["pageSize"].toInt(10);
        int offset = (pageNum - 1) * pageSize;

        // 构造查询SQL
        QString sql = QString("SELECT o.*, u.telephone, u.username %1 ORDER BY o.create_time DESC LIMIT %2 OFFSET %3")
                          .arg(sqlBase)
                          .arg(pageSize)
                          .arg(offset);

        qDebug() << "[DAO-DEBUG] 分页查询SQL:" << sql;

        QJsonArray arr;
        if (query.exec(sql)) {
            while (query.next()) {
                QJsonObject o;
                o["orderId"] = query.value("order_id").toString();
                o["userId"] = query.value("user_id").toInt();
                o["username"] = query.value("username").toString();
                o["telephone"] = query.value("telephone").toString();
                o["flightId"] = query.value("flight_id").toInt();
                o["totalPrice"] = query.value("total_price").toInt();
                o["status"] = query.value("status").toString();
                o["createTime"] = query.value("create_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");

                // 可选：添加航班信息（如果需要的话）
                // o["flightNumber"] = query.value("flight_number").toString();
                // o["departureCity"] = query.value("departure_city").toString();

                arr.append(o);
            }
        } else {
            qDebug() << "[DAO-DEBUG] 订单查询SQL执行失败:" << query.lastError().text();
        }

        qDebug() << "[DAO-DEBUG] 查询完成, 总数:" << total << "当前条数:" << arr.size();

        QJsonObject res;
        res["total"] = total;
        res["pageNum"] = pageNum;
        res["pageSize"] = pageSize;
        res["list"] = arr;
        return res;
    }
};

#endif // ADMINDAO_H
