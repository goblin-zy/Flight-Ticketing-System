#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include "DBManager.h"
#include "FlightDao.h"
#include "OrderInfoDao.h"
#include "Flight.h"
#include "OrderInfo.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ========== 1. 初始化数据库连接 ==========
    DBManager::getInstance();
    qDebug() << "\n------------------------ 测试开始 ------------------------\n";

    // ========== 2. 测试参数配置（替换为你数据库中的真实ID） ==========
    const int TEST_USER_ID = 1;        // 测试用户ID（需在user表中存在）
    const int TEST_FLIGHT_ID = 1;      // 测试航班ID（需在flight表中存在）
    const int TEST_TICKET_NUM = 1;     // 测试购票数量

    // ========== 3. 第一步：查询航班信息（为创建订单准备） ==========
    qDebug() << "【1. 测试FlightDao - 查询航班】";
    FlightDao flightDao;
    Flight* flight = flightDao.getFlightById(TEST_FLIGHT_ID);

    if (!flight) {
        qDebug() << "❌ 航班ID=" << TEST_FLIGHT_ID << "不存在，终止测试";
        qDebug() << "\n------------------------ 测试结束 ------------------------";
        return a.exec();
    }

    // 打印航班信息
    qDebug() << "✅ 查询到航班信息：";
    qDebug() << "   航班号：" << flight->flightNumber();
    qDebug() << "   出发地：" << flight->departureCity() << " → 目的地：" << flight->arrivalCity();
    qDebug() << "   单张票价：¥" << flight->price();
    qDebug() << "   当前余票：" << flight->leftTickets() << "张";

    // 扣减航班余票（下单前置操作）
    bool reduceOk = flightDao.reduceLeftTickets(TEST_FLIGHT_ID, TEST_TICKET_NUM);
    if (!reduceOk) {
        qDebug() << "❌ 航班余票扣减失败，终止测试";
        delete flight;
        qDebug() << "\n------------------------ 测试结束 ------------------------";
        return a.exec();
    }
    qDebug() << "✅ 航班余票扣减成功，剩余余票：" << (flight->leftTickets() - TEST_TICKET_NUM) << "张\n";

    // ========== 4. 第二步：测试OrderInfoDao - 创建订单（生成ORD格式ID） ==========
    qDebug() << "【2. 测试OrderInfoDao - 创建订单】";
    OrderInfoDao orderDao;
    OrderInfo* newOrder = new OrderInfo();

    // 计算订单总价
    double totalPrice = flight->price() * TEST_TICKET_NUM;

    // 给订单赋值（不含orderId，由DAO自动生成）
    newOrder->setUserId(TEST_USER_ID);
    newOrder->setFlightId(TEST_FLIGHT_ID);
    newOrder->setTotalPrice(totalPrice);
    newOrder->setStatus("待支付");
    newOrder->setCreateTime(QDateTime::currentDateTime());

    // 调用addOrder创建订单（自动生成ORD202512010001格式ID）
    bool addOrderOk = orderDao.addOrder(newOrder);
    if (!addOrderOk) {
        qDebug() << "❌ 订单创建失败";
        delete flight;
        delete newOrder;
        qDebug() << "\n------------------------ 测试结束 ------------------------";
        return a.exec();
    }

    // ========== 5. 第三步：测试OrderInfoDao - 按用户ID查询订单 ==========
    qDebug() << "\n【3. 测试OrderInfoDao - 查询用户订单】";
    QList<OrderInfo*> orderList = orderDao.getOrderByUserId(TEST_USER_ID);

    if (orderList.isEmpty()) {
        qDebug() << "❌ 用户ID=" << TEST_USER_ID << "暂无订单";
    } else {
        qDebug() << "✅ 查询到用户ID=" << TEST_USER_ID << "的订单列表（共" << orderList.size() << "条）：";
        for (int i = 0; i < orderList.size(); i++) {
            OrderInfo* order = orderList.at(i);
            qDebug() << "   订单" << (i+1) << "：";
            qDebug() << "      订单ID：" << order->orderId();       // 打印ORD格式订单ID
            qDebug() << "      关联航班ID：" << order->flightId();
            qDebug() << "      订单总价：¥" << order->totalPrice();
            qDebug() << "      订单状态：" << order->status();
            qDebug() << "      创建时间：" << order->createTime().toString("yyyy-MM-dd hh:mm:ss");
        }
    }

    // 新增：测试修改订单状态
    qDebug() << "\n【4. 测试OrderInfoDao - 修改订单状态】";
    QString targetOrderId = "ORD202512150001"; // 替换为实际生成的订单ID
    bool updateOk = orderDao.updateOrderStatus(targetOrderId, "已支付");
    if (updateOk) {
        qDebug() << "✅ 订单ID=" << targetOrderId << "状态更新为「已支付」成功";
    } else {
        qDebug() << "❌ 订单状态更新失败";
    }

    // ========== 6. 内存释放（避免内存泄漏） ==========
    delete flight;
    delete newOrder;
    orderDao.releaseOrderList(orderList); // 释放查询到的订单列表

    // ========== 7. 测试收尾 ==========
    qDebug() << "\n------------------------ 测试结束 ------------------------";

    return a.exec();
}
