#ifndef FLIGHTDAO_H
#define FLIGHTDAO_H

#include "DBManager.h"
#include "Flight.h"
#include <QList>
#include <QSqlQuery>
#include <QDebug>

class FlightDao {
public:
    // 按航班ID查询航班（下单核心）
    Flight* getFlightById(int flightId) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT * FROM flight WHERE flight_id = ?");
        query.addBindValue(flightId);

        if (query.exec() && query.next()) {
            Flight* flight = new Flight();
            flight->setFlightId(query.value("flight_id").toInt());
            flight->setFlightNumber(query.value("flight_number").toString());
            flight->setDepartureCity(query.value("departure_city").toString());
            flight->setArrivalCity(query.value("arrival_city").toString());
            // QDateTime直接赋值（已和Flight类匹配）
            flight->setDepartureTime(query.value("departure_time").toDateTime());
            flight->setArrivalTime(query.value("arrival_time").toDateTime());
            flight->setCheckinStartTime(query.value("checkin_start_time").toDateTime());
            flight->setCheckinEndTime(query.value("checkin_end_time").toDateTime());
            flight->setPrice(query.value("price").toDouble());
            flight->setLeftTickets(query.value("left_tickets").toInt());
            return flight;
        }
        qDebug() << "查询航班失败：" << query.lastError().text();
        return nullptr;
    }

    // 按出发/到达城市查询航班（用户订票查询核心）
    QList<Flight*> getFlightByCity(const QString& depCity, const QString& arrCity) {
        QList<Flight*> flightList;
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT * FROM flight WHERE departure_city = ? AND arrival_city = ?");
        query.addBindValue(depCity);
        query.addBindValue(arrCity);

        if (query.exec()) {
            while (query.next()) {
                Flight* flight = new Flight();
                flight->setFlightId(query.value("flight_id").toInt());
                flight->setFlightNumber(query.value("flight_number").toString());
                flight->setDepartureCity(query.value("departure_city").toString());
                flight->setArrivalCity(query.value("arrival_city").toString());
                flight->setDepartureTime(query.value("departure_time").toDateTime());
                flight->setArrivalTime(query.value("arrival_time").toDateTime());
                flight->setCheckinStartTime(query.value("checkin_start_time").toDateTime());
                flight->setCheckinEndTime(query.value("checkin_end_time").toDateTime());
                flight->setPrice(query.value("price").toDouble());
                flight->setLeftTickets(query.value("left_tickets").toInt());
                flightList.append(flight);
            }
        } else {
            qDebug() << "查询航班列表失败：" << query.lastError().text();
        }
        return flightList;
    }

    // 扣减航班余票（下单核心，含余票校验）
    bool reduceLeftTickets(int flightId, int num) {
        // 先查询余票，避免超卖
        Flight* flight = getFlightById(flightId);
        if (!flight) {
            qDebug() << "航班不存在";
            return false;
        }
        if (flight->leftTickets() < num) {
            qDebug() << "余票不足：当前余票" << flight->leftTickets() << "需扣减" << num;
            delete flight;
            return false;
        }
        delete flight;

        // 执行扣减
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("UPDATE flight SET left_tickets = left_tickets - ? WHERE flight_id = ?");
        query.addBindValue(num);
        query.addBindValue(flightId);

        if (query.exec()) {
            return true;
        } else {
            qDebug() << "扣减余票失败：" << query.lastError().text();
            return false;
        }
    }

    // 退票反向增加余票数
    bool addLeftTickets(int flightId, int num) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("UPDATE flight SET left_tickets = left_tickets + ? WHERE flight_id = ?");
        query.addBindValue(num);
        query.addBindValue(flightId);
        return query.exec();
    }

    // 释放QList<Flight*>内存的工具方法
    void releaseFlightList(QList<Flight*>& flightList) {
        for (Flight* flight : flightList) {
            delete flight;
        }
        flightList.clear();
    }
};

#endif // FLIGHTDAO_H
