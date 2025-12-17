#ifndef ORDERINFODAO_H
#define ORDERINFODAO_H

#include "DBManager.h"
#include "OrderInfo.h"
#include <QList>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QString>

class OrderInfoDao {
public:
    // 新增订单（核心：生成ORD+日期+序号格式订单ID）
    bool addOrder(OrderInfo* order) {
        QSqlQuery query(DBManager::getInstance().getConnection());

        // ========== 1. 生成指定格式的订单ID（ORD+年月日+4位序号） ==========
        QString orderId = generateOrderId();
        if (orderId.isEmpty()) { // 生成失败则直接返回
            qDebug() << "订单ID生成失败！";
            return false;
        }

        // ========== 2. 插入SQL包含order_id字段（适配varchar类型） ==========
        query.prepare("INSERT INTO order_info (order_id, user_id, flight_id, total_price, status, create_time) VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(orderId);          // 手动生成的ORD格式订单ID
        query.addBindValue(order->userId());  // 用户ID
        query.addBindValue(order->flightId());// 航班ID
        query.addBindValue(order->totalPrice());// 订单总价
        query.addBindValue(order->status());  // 订单状态（如"待支付"）
        query.addBindValue(order->createTime());// 创建时间

        // ========== 3. 执行插入并返回结果 ==========
        if (query.exec()) {
            qDebug() << "✅ 订单创建成功，订单ID：" << orderId;
            return true;
        } else {
            qDebug() << "❌ 新增订单失败：" << query.lastError().text();
            return false;
        }
    }

    // 按用户ID查询所有订单（用户查订单核心）
    QList<OrderInfo*> getOrderByUserId(int userId) {
        QList<OrderInfo*> orderList;
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT * FROM order_info WHERE user_id = ? ORDER BY create_time DESC");
        query.addBindValue(userId);

        if (query.exec()) {
            while (query.next()) {
                OrderInfo* order = new OrderInfo();
                // 注意：order_id是varchar类型，读取后转QString
                order->setOrderId(query.value("order_id").toString());
                order->setUserId(query.value("user_id").toInt());
                order->setFlightId(query.value("flight_id").toInt());
                order->setTotalPrice(query.value("total_price").toDouble());
                order->setStatus(query.value("status").toString());
                order->setCreateTime(query.value("create_time").toDateTime());
                orderList.append(order);
            }
        } else {
            qDebug() << "查询订单列表失败：" << query.lastError().text();
        }
        return orderList;
    }

    // 修改订单状态（支付/退票核心）
    bool updateOrderStatus(const QString& orderId, const QString& status) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("UPDATE order_info SET status = ? WHERE order_id = ?");
        query.addBindValue(status);
        query.addBindValue(orderId); // 适配varchar类型的order_id

        if (query.exec()) {
            return true;
        } else {
            qDebug() << "修改订单状态失败：" << query.lastError().text();
            return false;
        }
    }

    // 释放QList<OrderInfo*>内存的工具方法
    void releaseOrderList(QList<OrderInfo*>& orderList) {
        for (OrderInfo* order : orderList) {
            delete order;
        }
        orderList.clear();
    }

private:
    // 核心工具方法：生成ORD+年月日+4位序号的唯一订单ID（如ORD202512010001）
    QString generateOrderId() {
        // 1. 获取当前日期（格式：20251201）
        QString dateStr = QDateTime::currentDateTime().toString("yyyyMMdd");

        // 2. 查询当日已生成的最大订单序号（保证序号递增）
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT MAX(RIGHT(order_id, 4)) FROM order_info WHERE order_id LIKE ?");
        query.addBindValue(QString("ORD%1%").arg(dateStr)); // 匹配当日的订单ID

        int maxSeq = 0;
        if (query.exec() && query.next() && !query.isNull(0)) {
            maxSeq = query.value(0).toInt(); // 读取当日最大序号
        }

        // 3. 生成新序号（+1后补4位，不足补0）
        int newSeq = maxSeq + 1;
        QString seqStr = QString("%1").arg(newSeq, 4, 10, QChar('0')); // 0001、0002...

        // 4. 拼接最终订单ID（如ORD202512010001）
        return QString("ORD%1%2").arg(dateStr).arg(seqStr);
    }
};

#endif // ORDERINFODAO_H
