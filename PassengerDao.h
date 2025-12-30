#ifndef PASSENGERDAO_H
#define PASSENGERDAO_H
#include "Passenger.h"
#include"DBManager.h"
#include <QList>
#include <QSqlQuery>
#include <QDebug>

class PassengerDao {
public:
    // 添加乘客
    bool addPassenger(Passenger* passenger) {
        if (passenger->orderId().isEmpty() || passenger->name().isEmpty() || passenger->idCard().isEmpty()) {
            qDebug() << "乘客信息不完整";
            return false;
        }
        QSqlQuery query;
        query.prepare("INSERT INTO passenger (order_id, name, id_card) VALUES (?, ?, ?)");
        query.addBindValue(passenger->orderId());
        query.addBindValue(passenger->name());
        query.addBindValue(passenger->idCard());
        return query.exec();
    }

    // 按订单ID查询乘客
    QList<Passenger*> getPassengerByOrderId(const QString& orderId) {
        QList<Passenger*> passengerList;
        QSqlQuery query;
        query.prepare("SELECT * FROM passenger WHERE order_id = ?");
        query.addBindValue(orderId);
        if (query.exec()) {
            while (query.next()) {
                Passenger* p = new Passenger();
                p->setOrderId(query.value("order_id").toString());
                p->setName(query.value("name").toString());
                p->setIdCard(query.value("id_card").toString());
                passengerList.append(p);
            }
        }
        return passengerList;
    }

    int getPassengerCountByOrderId(const QString& orderId) {
        QSqlQuery query(DBManager::getInstance().getConnection());
        query.prepare("SELECT COUNT(*) FROM passenger WHERE order_id = ?");
        query.addBindValue(orderId);
        if (query.exec() && query.next()) {
            return query.value(0).toInt();
        }
        return 0;
    }


    // 按订单ID删除乘客
    bool deletePassengerByOrderId(const QString& orderId) {
        QSqlQuery query;
        query.prepare("DELETE FROM passenger WHERE order_id = ?");
        query.addBindValue(orderId);
        return query.exec();
    }


    // 释放乘客列表内存
    void releasePassengerList(QList<Passenger*>& passengerList) {
        for (Passenger* p : passengerList) {
            delete p;
        }
        passengerList.clear();
    }
};

#endif // PASSENGERDAO_H
