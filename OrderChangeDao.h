#ifndef ORDERCHANGEDAO_H
#define ORDERCHANGEDAO_H

#include "DBManager.h"
#include "OrderChange.h"
#include "OrderInfoDao.h"
#include "FlightDao.h"
#include "PassengerDao.h"
#include <QSqlQuery>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>  // 新增
#include <QDateTime>      // 新增

class OrderChangeDao {
public:
    // 1.3.4 改签
    QJsonObject changeOrder(const QString& orderId, int newFlightId) {
        QJsonObject response;

        // 获取数据库连接
        QSqlDatabase db = DBManager::getInstance().getConnection();
        if (!db.isOpen()) {
            response["code"] = 500;
            response["message"] = "数据库连接失败";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 开始事务
        if (!db.transaction()) {
            response["code"] = 500;
            response["message"] = "事务启动失败";
            response["data"] = QJsonValue::Null;
            return response;
        }

        try {
            // 1. 校验原订单
            QSqlQuery query(db);
            query.prepare("SELECT flight_id, user_id, total_price, status FROM order_info WHERE order_id = ?");
            query.addBindValue(orderId);

            if (!query.exec()) {
                throw QString("查询订单失败: ") + query.lastError().text();
            }

            if (!query.next()) {
                throw QString("订单不存在或未支付");
            }

            int oldFlightId = query.value("flight_id").toInt();
            int userId = query.value("user_id").toInt();
            int oldTotalPrice = query.value("total_price").toInt();
            QString orderStatus = query.value("status").toString();

            // 检查订单状态
            if (orderStatus != "已支付") {
                throw QString("仅已支付的订单可以改签");
            }

            // 2. 校验新航班
            FlightDao flightDao;
            Flight* newFlight = flightDao.getFlightById(newFlightId);
            if (!newFlight) {
                throw QString("目标航班不存在");
            }

            // 3. 获取乘客数量
            PassengerDao passengerDao;
            int passengerCount = passengerDao.getPassengerCountByOrderId(orderId);
            if (passengerCount <= 0) {
                throw QString("订单没有乘客信息");
            }

            if (newFlight->leftTickets() < passengerCount) {
                delete newFlight;
                throw QString("目标航班余票不足");
            }

            // 4. 计算差价
            int newUnitPrice = newFlight->price();
            int newTotalPrice = newUnitPrice * passengerCount;
            int priceDifference = newTotalPrice - oldTotalPrice;

            // 5. 原航班恢复余票
            if (!flightDao.addLeftTickets(oldFlightId, passengerCount)) {
                throw QString("原航班余票恢复失败");
            }

            // 6. 新航班扣减余票
            if (!flightDao.reduceLeftTickets(newFlightId, passengerCount)) {
                throw QString("新航班余票扣减失败");
            }

            // 7. 更新订单信息
            query.prepare(R"(
                UPDATE order_info
                SET flight_id = ?, total_price = ?, status = '改签完成'
                WHERE order_id = ?
            )");
            query.addBindValue(newFlightId);
            query.addBindValue(newTotalPrice);
            query.addBindValue(orderId);

            if (!query.exec()) {
                throw QString("更新订单失败: ") + query.lastError().text();
            }

            // 8. 记录改签历史
            query.prepare(R"(
                INSERT INTO order_change
                (order_id, old_flight_id, new_flight_id, price_difference, change_time)
                VALUES (?, ?, ?, ?, ?)
            )");
            query.addBindValue(orderId);
            query.addBindValue(oldFlightId);
            query.addBindValue(newFlightId);
            query.addBindValue(priceDifference);
            query.addBindValue(QDateTime::currentDateTime());

            if (!query.exec()) {
                throw QString("记录改签历史失败: ") + query.lastError().text();
            }

            // 提交事务
            if (!db.commit()) {
                throw QString("事务提交失败");
            }

            // 9. 构建返回数据
            QJsonObject data;
            data["orderId"] = orderId;
            data["status"] = "改签完成";
            data["newFlightNumber"] = newFlight->flightNumber();
            data["newDepartureCity"] = newFlight->departureCity();
            data["newArrivalCity"] = newFlight->arrivalCity();

            // 拼接日期和时间
            QString newDepartureTime = QString("%1 %2")
                                           .arg(newFlight->date().toString("yyyy-MM-dd"))
                                           .arg(newFlight->departureTime().toString("HH:mm"));
            data["newDepartureTime"] = newDepartureTime;

            data["priceDifference"] = priceDifference;

            response["code"] = 200;
            response["message"] = "改签成功";
            response["data"] = data;

            // 清理资源
            delete newFlight;

        } catch (const QString& error) {
            // 回滚事务
            db.rollback();

            response["code"] = 500;
            response["message"] = error;
            response["data"] = QJsonValue::Null;

            qDebug() << "改签失败:" << error;
        }

        return response;
    }

    // 查询订单改签记录（保持不变）
    QList<OrderChange*> getChangeByOrderId(const QString& orderId) {
        QList<OrderChange*> changeList;
        QSqlDatabase db = DBManager::getInstance().getConnection();
        QSqlQuery query(db);

        query.prepare("SELECT * FROM order_change WHERE order_id = ? ORDER BY change_time DESC");
        query.addBindValue(orderId);

        if (query.exec()) {
            while (query.next()) {
                OrderChange* change = new OrderChange();
                change->setChangeId(query.value("change_id").toInt());
                change->setOrderId(query.value("order_id").toString());
                change->setOldFlightId(query.value("old_flight_id").toInt());
                change->setNewFlightId(query.value("new_flight_id").toInt());
                change->setPriceDifference(query.value("price_difference").toInt());
                change->setChangeTime(query.value("change_time").toDateTime());
                changeList.append(change);
            }
        } else {
            qDebug() << "查询改签记录失败：" << query.lastError().text();
        }
        return changeList;
    }

    // 释放改签列表内存（保持不变）
    void releaseChangeList(QList<OrderChange*>& changeList) {
        for (OrderChange* change : changeList) {
            delete change;
        }
        changeList.clear();
    }
};

#endif // ORDERCHANGEDAO_H
