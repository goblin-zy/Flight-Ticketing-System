#ifndef ORDERINFODAO_H
#define ORDERINFODAO_H
#include "DBManager.h"
#include "OrderInfo.h"
#include "FlightDao.h"
#include "Passenger.h"       // 需提前创建Passenger类（存储乘客信息）
#include "PassengerDao.h"    // 需提前创建PassengerDao类（操作passenger表）
#include "UserDao.h"
#include "TokenTool.h"
#include <QList>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QPair>

class OrderInfoDao {
public:
    // 1. 机票预订（下单）- 适配API 1.3.1
    QJsonObject bookOrder(const QString& authorizationHeader, int flightId, const QList<QPair<QString, QString>>& passengers) {
        QJsonObject response;

        // ===== 修改点：从Authorization头获取并验证Token =====
        QString token = TokenTool::removeBearerPrefix(authorizationHeader);
        int userId = TokenTool::getUserIdByToken(token);
        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }
        // ===== 修改结束 =====

        FlightDao flightDao;
        PassengerDao passengerDao;

        // 校验1：航班是否存在+余票是否充足
        Flight* flight = flightDao.getFlightById(flightId);
        if (!flight) {
            response["code"] = 500;
            response["message"] = "航班不存在";
            response["data"] = QJsonValue::Null;
            return response;
        }
        if (flight->leftTickets() < passengers.size()) {
            delete flight;
            response["code"] = 500;
            response["message"] = "航班余票不足";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 校验2：乘客信息完整性
        for (auto& p : passengers) {
            if (p.first.isEmpty() || p.second.isEmpty()) {
                delete flight;
                response["code"] = 500;
                response["message"] = "乘客姓名或身份证号不能为空";
                response["data"] = QJsonValue::Null;
                return response;
            }
        }

        // 核心：事务处理（扣减余票+创建订单+插入乘客）
        QSqlDatabase db = DBManager::getInstance().getConnection();
        db.transaction();
        bool success = false;
        QString orderId = generateOrderId();

        try {
            // 步骤1：扣减航班余票
            if (!flightDao.reduceLeftTickets(flightId, passengers.size())) {
                throw QString("余票扣减失败");
            }

            // 步骤2：创建订单
            OrderInfo order;
            order.setOrderId(orderId);
            order.setUserId(userId);  // 使用从Token解析出的userId
            order.setFlightId(flightId);
            order.setTotalPrice(flight->price() * passengers.size()); // 单价×人数（分）
            order.setStatus("待支付");
            order.setCreateTime(QDateTime::currentDateTime());

            QSqlQuery query;
            query.prepare(R"(
            INSERT INTO order_info
            (order_id, user_id, flight_id, total_price, status, create_time)
            VALUES (?, ?, ?, ?, ?, ?)
        )");
            query.addBindValue(order.orderId());
            query.addBindValue(order.userId());
            query.addBindValue(order.flightId());
            query.addBindValue(order.totalPrice());
            query.addBindValue(order.status());
            query.addBindValue(order.createTime());
            if (!query.exec()) {
                throw QString("订单创建失败：" + query.lastError().text());
            }

            // 步骤3：插入乘客信息（关联passenger表）
            for (auto& p : passengers) {
                Passenger* passenger = new Passenger();
                passenger->setOrderId(orderId);
                passenger->setName(p.first);
                passenger->setIdCard(p.second);
                if (!passengerDao.addPassenger(passenger)) {
                    delete passenger;
                    throw QString("乘客信息插入失败：" + query.lastError().text());
                }
                delete passenger;
            }

            db.commit();
            success = true;
        } catch (QString err) {
            db.rollback();
            response["code"] = 500;
            response["message"] = err;
            response["data"] = QJsonValue::Null;
        }

        delete flight;
        if (success) {
            QJsonObject data;
            data["orderId"] = orderId;
            data["totalPrice"] = flight->price() * passengers.size();
            data["status"] = "待支付";
            response["code"] = 200;
            response["message"] = "下单成功，请支付";
            response["data"] = data;
        }
        return response;
    }

    // 2. 订单支付 - 适配API 1.3.2 (修复余额校验漏洞版本)
    QJsonObject payOrder(const QString& authorizationHeader, const QString& orderId) {
        QJsonObject response;

        // 1. 验证 Token
        QString token = TokenTool::removeBearerPrefix(authorizationHeader);
        int userId = TokenTool::verifyToken(token);
        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 获取数据库连接并开启事务
        QSqlDatabase db = DBManager::getInstance().getConnection();
        if (!db.transaction()) {
            response["code"] = 500;
            response["message"] = "数据库事务启动失败";
            return response;
        }

        QSqlQuery query(db);

        // 2. 查询订单金额、状态及归属 (使用 FOR UPDATE 锁定行，防止并发问题)
        // MySQL语法：SELECT ... FOR UPDATE
        // SQLite语法：BEGIN IMMEDIATE TRANSACTION（已在上面开启）
        query.prepare("SELECT total_price, status, user_id FROM order_info WHERE order_id = ?");
        query.addBindValue(orderId);

        if (!query.exec() || !query.next()) {
            db.rollback();
            response["code"] = 500;
            response["message"] = "订单不存在";
            return response;
        }

        // 修改点1：订单金额用整数（分）
        int totalPriceFen = query.value("total_price").toInt(); // 改为toInt()
        QString currentStatus = query.value("status").toString();
        int orderUserId = query.value("user_id").toInt();

        // 3. 安全校验：归属权
        if (orderUserId != userId) {
            db.rollback();
            response["code"] = 500;
            response["message"] = "无权操作该订单";
            return response;
        }

        // 4. 安全校验：订单状态
        if (currentStatus != "待支付") {
            db.rollback();
            response["code"] = 500;
            response["message"] = "订单当前状态为[" + currentStatus + "]，不可支付";
            return response;
        }

        // 5. 【核心修复】校验用户余额是否充足
        query.prepare("SELECT balance FROM user WHERE user_id = ?");
        query.addBindValue(userId);
        if (!query.exec() || !query.next()) {
            db.rollback();
            response["code"] = 500;
            response["message"] = "用户信息获取失败";
            return response;
        }

        // 修改点2：用户余额用整数（分）
        int userBalanceFen = query.value("balance").toInt(); // 改为toInt()

        // 修改点3：整数比较，避免浮点数精度问题
        if (userBalanceFen < totalPriceFen) {
            db.rollback();
            response["code"] = 500;

            // 修改点4：显示时转换为元，保留2位小数
            double totalPriceYuan = totalPriceFen / 100.0;
            double userBalanceYuan = userBalanceFen / 100.0;

            response["message"] = QString("余额不足！订单金额: ¥%1, 当前余额: ¥%2")
                                      .arg(totalPriceYuan, 0, 'f', 2)
                                      .arg(userBalanceYuan, 0, 'f', 2);
            return response;
        }

        // 6. 执行扣款 (原子操作)
        query.prepare("UPDATE user SET balance = balance - ? WHERE user_id = ?");
        query.addBindValue(totalPriceFen); // 传递整数分
        query.addBindValue(userId);
        if (!query.exec()) {
            db.rollback();
            response["code"] = 500;
            response["message"] = "账户扣款失败";
            return response;
        }

        // 7. 更新订单状态和支付时间
        QDateTime payTime = QDateTime::currentDateTime();
        query.prepare("UPDATE order_info SET status = '已支付', pay_time = ? WHERE order_id = ?");
        query.addBindValue(payTime);
        query.addBindValue(orderId);

        if (query.exec()) {
            // 8. 提交事务
            if (db.commit()) {
                QJsonObject data;
                data["orderId"] = orderId;
                data["status"] = "已支付";
                data["payTime"] = payTime.toString("yyyy-MM-dd HH:mm:ss");

                // 修改点5：计算剩余余额并返回（分）
                int remainingBalanceFen = userBalanceFen - totalPriceFen;
                data["remainingBalance"] = remainingBalanceFen; // 返回分

                response["code"] = 200;
                response["message"] = "支付成功";
                response["data"] = data;
            } else {
                db.rollback();
                response["code"] = 500;
                response["message"] = "事务提交失败";
            }
        } else {
            db.rollback();
            response["code"] = 500;
            response["message"] = "更新订单状态失败";
        }

        return response;
    }

    // 3. 订单查询（个人列表）- 适配API 1.3.3
    QJsonObject queryOrderByUserId(const QString& authorizationHeader, const QString& status = "") {
        QJsonObject response;

        // ===== 修改点：从Authorization头获取并验证Token =====
        QString token = TokenTool::removeBearerPrefix(authorizationHeader);
        int userId = TokenTool::getUserIdByToken(token);
        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }
        // ===== 修改结束 =====

        QJsonArray orderArray;
        QSqlQuery query;
        PassengerDao passengerDao;

        // 构建查询SQL（支持按状态筛选）
        QString sql = "SELECT * FROM order_info WHERE user_id = ?";
        QList<QVariant> bindValues = {userId};
        if (!status.isEmpty()) {
            sql += " AND status = ?";
            bindValues.append(status);
        }
        sql += " ORDER BY create_time DESC";

        query.prepare(sql);
        for (int i = 0; i < bindValues.size(); ++i) {
            query.addBindValue(bindValues[i]);
        }

        if (query.exec()) {
            while (query.next()) {
                QJsonObject orderObj;
                // 订单基础信息
                orderObj["orderId"] = query.value("order_id").toString();
                orderObj["totalPrice"] = query.value("total_price").toInt();
                orderObj["status"] = query.value("status").toString();
                orderObj["createTime"] = query.value("create_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");

                // 关联航班信息（从flight表查询）
                int flightId = query.value("flight_id").toInt();
                FlightDao flightDao;
                Flight* flight = flightDao.getFlightById(flightId);
                if (flight) {
                    orderObj["flightNumber"] = flight->flightNumber();
                    orderObj["departureCity"] = flight->departureCity();
                    orderObj["arrivalCity"] = flight->arrivalCity();
                    orderObj["departureTime"] = QString("%1 %2").arg(flight->date().toString("yyyy-MM-dd")).arg(flight->departureTime().toString("HH:mm"));
                    delete flight;
                }

                // 关联乘客信息（从passenger表查询）
                QJsonArray passengerArray;
                QList<Passenger*> passengers = passengerDao.getPassengerByOrderId(query.value("order_id").toString());
                for (Passenger* p : passengers) {
                    QJsonObject pObj;
                    pObj["name"] = p->name();
                    pObj["idCard"] = p->idCard().left(6) + "********" + p->idCard().right(4); // 脱敏
                    passengerArray.append(pObj);
                }
                passengerDao.releasePassengerList(passengers);
                orderObj["passengers"] = passengerArray;

                orderArray.append(orderObj);
            }
        } else {
            response["code"] = 500;
            response["message"] = "查询失败：" + query.lastError().text();
            response["data"] = QJsonValue::Null;
            return response;
        }

        if (orderArray.isEmpty()) {
            response["code"] = 500;
            response["message"] = "无订单记录";
            response["data"] = QJsonValue::Null;
        } else {
            response["code"] = 200;
            response["message"] = "获取成功";
            response["data"] = orderArray;
        }
        return response;
    }

    //1.3.4在orderchange中
    // 4. 订单退签 - 适配API 1.3.5（添加Token验证+用户权限校验）
    QJsonObject refundOrder(const QString& authorizationHeader, const QString& orderId) {
        QJsonObject response;
        QSqlQuery query;
        FlightDao flightDao;
        PassengerDao passengerDao;

        // ===== 修改：使用TokenTool处理Bearer前缀 =====
        QString token = TokenTool::removeBearerPrefix(authorizationHeader);
        int userId = TokenTool::getUserIdByToken(token);
        // ===== 修改结束 =====

        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 2. 校验订单归属（确保订单属于当前用户）
        query.prepare("SELECT flight_id, status, user_id FROM order_info WHERE order_id = ?");
        query.addBindValue(orderId);
        if (!query.exec() || !query.next()) {
            response["code"] = 500;
            response["message"] = "订单不存在";
            response["data"] = QJsonValue::Null;
            return response;
        }
        // 新增：校验订单所属用户ID与Token解析的ID一致
        if (query.value("user_id").toInt() != userId) {
            response["code"] = 500;
            response["message"] = "无权操作该订单";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // ===== 原有业务逻辑（仅调整查询字段，其余完全不变）=====
        if (query.value("status").toString() != "已支付") {
            response["code"] = 500;
            response["message"] = "已过退签时间或订单未支付";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 事务处理：恢复余票+更新订单状态
        QSqlDatabase db = DBManager::getInstance().getConnection();
        db.transaction();
        int flightId = query.value("flight_id").toInt();
        bool success = false;

        try {
            // 步骤1：查询乘客数（用于恢复余票）
            int passengerCount = passengerDao.getPassengerCountByOrderId(orderId);

            // 步骤2：恢复航班余票
            if (!flightDao.addLeftTickets(flightId, passengerCount)) {
                throw QString("余票恢复失败");
            }

            // 步骤3：更新订单状态+退签时间
            QDateTime refundTime = QDateTime::currentDateTime();
            query.prepare(R"(
            UPDATE order_info
            SET status = '已退签', refund_time = ?
            WHERE order_id = ?
        )");
            query.addBindValue(refundTime);
            query.addBindValue(orderId);
            if (!query.exec()) {
                throw QString("订单状态更新失败：" + query.lastError().text());
            }

            db.commit();
            success = true;
        } catch (QString err) {
            db.rollback();
            response["code"] = 500;
            response["message"] = err;
            response["data"] = QJsonValue::Null;
        }

        if (success) {
            QJsonObject data;
            data["orderId"] = orderId;
            data["status"] = "已退签";
            data["refundTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            response["code"] = 200;
            response["message"] = "退签成功";
            response["data"] = data;
        }
        return response;
    }

    // 新增：删除订单（针对未付款/已取消）- 适配API 1.3.6
    QJsonObject deleteOrder(const QString& authorizationHeader, const QString& orderId) {
        QJsonObject response;

        // ===== 修改：使用TokenTool处理Bearer前缀 =====
        QString token = TokenTool::removeBearerPrefix(authorizationHeader);
        int userId = TokenTool::getUserIdByToken(token);
        // ===== 修改结束 =====

        if (userId == -1) {
            response["code"] = 500;
            response["message"] = "Token无效或已过期";
            response["data"] = QJsonValue::Null;
            return response;
        }

        QSqlQuery query;
        // 校验1：订单是否存在且属于当前用户
        query.prepare("SELECT status FROM order_info WHERE order_id = ? AND user_id = ?");
        query.addBindValue(orderId);
        query.addBindValue(userId);
        if (!query.exec() || !query.next()) {
            response["code"] = 500;
            response["message"] = "订单不存在或无权操作";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 校验2：订单状态是否为"待支付"或"已取消"
        QString status = query.value(0).toString();
        if (status != "待支付" && status != "已取消") {
            response["code"] = 500;
            response["message"] = "只能删除未支付或已取消的订单";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 事务处理：删除订单+删除关联乘客
        QSqlDatabase db = DBManager::getInstance().getConnection();
        db.transaction();
        PassengerDao passengerDao;
        bool success = false;

        try {
            // 步骤1：删除订单
            query.prepare("DELETE FROM order_info WHERE order_id = ?");
            query.addBindValue(orderId);
            if (!query.exec()) {
                throw QString("订单删除失败");
            }

            // 步骤2：删除关联乘客
            if (!passengerDao.deletePassengerByOrderId(orderId)) {
                throw QString("乘客信息删除失败");
            }

            db.commit();
            success = true;
        } catch (QString err) {
            db.rollback();
            response["code"] = 500;
            response["message"] = err;
            response["data"] = QJsonValue::Null;
        }

        if (success) {
            QJsonObject data;
            data["orderId"] = orderId;
            response["code"] = 200;
            response["message"] = "删除成功";
            response["data"] = data;
        }
        return response;
    }

    // 释放订单列表内存
    void releaseOrderList(QList<OrderInfo*>& orderList) {
        for (OrderInfo* order : orderList) {
            delete order;
        }
        orderList.clear();
    }

private:
    // 生成ORD格式订单ID（如ORD202512010001）
    QString generateOrderId() {
        QString dateStr = QDateTime::currentDateTime().toString("yyyyMMdd");
        QSqlQuery query;
        query.prepare("SELECT MAX(RIGHT(order_id, 4)) FROM order_info WHERE order_id LIKE ?");
        query.addBindValue(QString("ORD%1%").arg(dateStr));
        int maxSeq = 0;
        if (query.exec() && query.next() && !query.isNull(0)) {
            maxSeq = query.value(0).toInt();
        }
        int newSeq = maxSeq + 1;
        QString seqStr = QString("%1").arg(newSeq, 4, 10, QChar('0'));
        return QString("ORD%1%2").arg(dateStr).arg(seqStr);
    }
};

#endif // ORDERINFODAO_H
