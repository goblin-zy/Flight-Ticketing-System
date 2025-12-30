#ifndef FLIGHTDAO_H
#define FLIGHTDAO_H

#include "Flight.h"
#include "DBManager.h"
#include <QList>
#include <QSqlQuery>
#include <QDate>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class FlightDao {
public:

    Flight* getFlightById(int flightId) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT * FROM flight WHERE flight_id = ?");
        query.addBindValue(flightId);

        if (query.exec() && query.next()) {
            Flight* flight = new Flight();
            // 严格映射数据库字段到Flight对象（与queryFlightsWithFilters中的逻辑一致）
            flight->setFlightId(query.value("flight_id").toInt());
            flight->setFlightNumber(query.value("flight_number").toString());
            flight->setDepartureCity(query.value("departure_city").toString());
            flight->setArrivalCity(query.value("arrival_city").toString());
            flight->setDate(query.value("date").toDate());
            flight->setDepartureTime(query.value("departure_time").toTime());
            flight->setArrivalTime(query.value("arrival_time").toTime());
            flight->setDepartureAirport(query.value("departure_airport").toString());
            flight->setArrivalAirport(query.value("arrival_airport").toString());
            flight->setCheckinStartTime(query.value("checkin_start_time").toTime());
            flight->setCheckinEndTime(query.value("checkin_end_time").toTime());
            flight->setPrice(query.value("price").toInt());
            flight->setAirlineCompany(query.value("airline_company").toString());
            flight->setStatus(query.value("status").toString());
            flight->setLeftTickets(query.value("left_tickets").toInt());
            return flight;
        }
        return nullptr; // 未找到航班时返回空
    }

    // ========== 新增：余票扣减（下单/改签时使用） ==========
    bool reduceLeftTickets(int flightId, int num) {
        if (num <= 0) {
            qDebug() << "扣减余票数量必须为正数";
            return false;
        }

        QSqlQuery query(DBManager::getInstance().getConnection());
        // 先查询余票，避免超卖
        query.prepare("SELECT left_tickets FROM flight WHERE flight_id = ?");
        query.addBindValue(flightId);
        if (!query.exec() || !query.next()) {
            qDebug() << "查询航班余票失败：" << query.lastError().text();
            return false;
        }

        int leftTickets = query.value("left_tickets").toInt();
        if (leftTickets < num) {
            qDebug() << "余票不足：当前" << leftTickets << "张，需扣减" << num << "张";
            return false;
        }

        // 执行扣减
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

    // ========== 新增：余票恢复（退签/改签时使用） ==========
    bool addLeftTickets(int flightId, int num) {
        if (num <= 0) {
            qDebug() << "恢复余票数量必须为正数";
            return false;
        }

        QSqlQuery query(DBManager::getInstance().getConnection());
        // 先校验航班是否存在
        query.prepare("SELECT flight_id FROM flight WHERE flight_id = ?");
        query.addBindValue(flightId);
        if (!query.exec() || !query.next()) {
            qDebug() << "航班不存在：flightId=" << flightId;
            return false;
        }

        // 执行恢复
        query.prepare("UPDATE flight SET left_tickets = left_tickets + ? WHERE flight_id = ?");
        query.addBindValue(num);
        query.addBindValue(flightId);
        if (query.exec()) {
            return true;
        } else {
            qDebug() << "恢复余票失败：" << query.lastError().text();
            return false;
        }
    }

    // 核心：带筛选+分页的航班查询（适配API参数）
    QList<Flight*> queryFlightsWithFilters(
        const QString& departureCity,  // 出发城市（可选）
        const QString& arrivalCity,    // 目的城市（可选）
        const QDate& date,             // 航班日期（可选）
        const QString& airlineCompany, // 航空公司（可选）
        int minPrice,                  // 最低价格（分，可选）
        int maxPrice,                  // 最高价格（分，可选）
        int pageNum,                   // 页码（必填）
        int pageSize                   // 每页条数（必填）
        ) {
        QList<Flight*> flightList;
        QSqlQuery query(DBManager::getInstance().getConnection());
        QString sql = "SELECT * FROM flight WHERE 1=1";
        QList<QVariant> bindValues;

        // 1. 出发城市筛选
        if (!departureCity.isEmpty()) {
            sql += " AND departure_city = ?";
            bindValues.append(departureCity);
        }

        // 2. 目的城市筛选
        if (!arrivalCity.isEmpty()) {
            sql += " AND arrival_city = ?";
            bindValues.append(arrivalCity);
        }

        // 3. 航班日期筛选（数据库中是date字段，单独日期）
        if (date.isValid()) {
            sql += " AND date = ?";
            bindValues.append(date);
        }

        // 4. 航空公司筛选
        if (!airlineCompany.isEmpty()) {
            sql += " AND airline_company = ?";
            bindValues.append(airlineCompany);
        }

        // 5. 价格区间筛选（分）
        if (minPrice >= 0) {
            sql += " AND price >= ?";
            bindValues.append(minPrice);
        }
        if (maxPrice > 0) {
            sql += " AND price <= ?";
            bindValues.append(maxPrice);
        }

        // 6. 分页处理（MySQL LIMIT 偏移量, 条数）
        int offset = (pageNum - 1) * pageSize;
        sql += " LIMIT ? OFFSET ?";
        bindValues.append(pageSize);
        bindValues.append(offset);

        // 绑定参数并执行查询
        query.prepare(sql);
        for (int i = 0; i < bindValues.size(); ++i) {
            query.bindValue(i, bindValues[i]);
        }

        if (query.exec()) {
            while (query.next()) {
                Flight* flight = new Flight();
                // 严格映射数据库字段到Flight对象
                flight->setFlightId(query.value("flight_id").toInt());
                flight->setFlightNumber(query.value("flight_number").toString());
                flight->setDepartureCity(query.value("departure_city").toString());
                flight->setArrivalCity(query.value("arrival_city").toString());
                flight->setDate(query.value("date").toDate());
                flight->setDepartureTime(query.value("departure_time").toTime());
                flight->setArrivalTime(query.value("arrival_time").toTime());
                flight->setDepartureAirport(query.value("departure_airport").toString());
                flight->setArrivalAirport(query.value("arrival_airport").toString());
                flight->setCheckinStartTime(query.value("checkin_start_time").toTime());
                flight->setCheckinEndTime(query.value("checkin_end_time").toTime());
                flight->setPrice(query.value("price").toInt());
                flight->setAirlineCompany(query.value("airline_company").toString());
                flight->setStatus(query.value("status").toString());
                flight->setLeftTickets(query.value("left_tickets").toInt());
                flightList.append(flight);
            }
        } else {
            qCritical() << "航班查询失败：" << query.lastError().text();
        }

        return flightList;
    }

    // 获取符合条件的航班总数（用于分页）
    int getFlightCountWithFilters(
        const QString& departureCity,
        const QString& arrivalCity,
        const QDate& date,
        const QString& airlineCompany,
        int minPrice,
        int maxPrice
        ) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        QString sql = "SELECT COUNT(*) FROM flight WHERE 1=1";
        QList<QVariant> bindValues;

        // 复用筛选条件（和查询方法一致）
        if (!departureCity.isEmpty()) {
            sql += " AND departure_city = ?";
            bindValues.append(departureCity);
        }
        if (!arrivalCity.isEmpty()) {
            sql += " AND arrival_city = ?";
            bindValues.append(arrivalCity);
        }
        if (date.isValid()) {
            sql += " AND date = ?";
            bindValues.append(date);
        }
        if (!airlineCompany.isEmpty()) {
            sql += " AND airline_company = ?";
            bindValues.append(airlineCompany);
        }
        if (minPrice >= 0) {
            sql += " AND price >= ?";
            bindValues.append(minPrice);
        }
        if (maxPrice > 0) {
            sql += " AND price <= ?";
            bindValues.append(maxPrice);
        }

        query.prepare(sql);
        for (int i = 0; i < bindValues.size(); ++i) {
            query.bindValue(i, bindValues[i]);
        }

        if (query.exec() && query.next()) {
            return query.value(0).toInt();
        } else {
            qCritical() << "查询航班总数失败：" << query.lastError().text();
            return 0;
        }
    }

    // 转换为API标准JSON格式（核心：完全匹配API返回结构）
    QJsonObject flightsToApiJson(QList<Flight*> flights, int totalCount, int pageNum, int pageSize) {
        QJsonObject response;
        QJsonObject dataObj;
        QJsonArray listArray;

        // 无匹配航班时返回失败JSON
        if (flights.isEmpty()) {
            response["code"] = 500;
            response["message"] = "无匹配航班";
            response["data"] = QJsonValue::Null;
            return response;
        }

        // 有数据时构造成功JSON
        // 1. 构造航班列表数组
        for (Flight* flight : flights) {
            QJsonObject flightObj;
            flightObj["flightId"] = flight->flightId();
            flightObj["flightNumber"] = flight->flightNumber();
            flightObj["departureCity"] = flight->departureCity();
            flightObj["arrivalCity"] = flight->arrivalCity();
            flightObj["departureTime"] = flight->departureTime().toString("HH:mm"); // 仅时分
            flightObj["arrivalTime"] = flight->arrivalTime().toString("HH:mm");     // 仅时分
            flightObj["departureAirport"] = flight->departureAirport();
            flightObj["arrivalAirport"] = flight->arrivalAirport();
            flightObj["checkinStartTime"] = flight->checkinStartTime().toString("HH:mm");
            flightObj["checkinEndTime"] = flight->checkinEndTime().toString("HH:mm");
            flightObj["price"] = flight->price(); // 分，整数
            flightObj["airlineCompany"] = flight->airlineCompany();
            flightObj["status"] = flight->status();
            flightObj["leftTickets"] = flight->leftTickets();
            listArray.append(flightObj);
        }

        // 2. 构造data对象（匹配API的data结构）
        dataObj["total"] = totalCount;
        dataObj["pageNum"] = pageNum;
        dataObj["pageSize"] = pageSize;
        dataObj["list"] = listArray;

        // 3. 构造最终响应
        response["code"] = 200;
        response["message"] = "查询成功";
        response["data"] = dataObj;

        return response;
    }

    // 释放航班列表内存（避免内存泄漏）
    void releaseFlightList(QList<Flight*>& flightList) {
        for (Flight* flight : flightList) {
            delete flight;
        }
        flightList.clear();
    }
};

#endif // FLIGHTDAO_H
