#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUrlQuery>
#include <QStringList>
#include <QNetworkRequest>

#include "UserDao.h"
#include "FlightDao.h"
#include "OrderInfoDao.h"
#include "OrderChangeDao.h"
#include "AdminService.h"
#include "TokenTool.h"
#include "DBManager.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr) : QTcpServer(parent)
    {
        DBManager::getInstance();
    }

    bool startServer(quint16 port)
    {
        if (listen(QHostAddress::Any, port)) {
            qDebug() << "服务器启动成功，端口：" << port;
            return true;
        } else {
            qDebug() << "服务器启动失败：" << errorString();
            return false;
        }
    }

protected:
    void incomingConnection(qintptr socketDescriptor) override
    {
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            handleClientRequest(socket);
        });
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }

private slots:
    void handleClientRequest(QTcpSocket *socket)
    {
        // ================ [新增：函数入口监控] ================
        qDebug() << "\n[HTTP-LOG] >>> 进入 handleClientRequest 函数";

        QByteArray data = socket->readAll();
        QString request = QString::fromUtf8(data);
        QStringList lines = request.split("\r\n");
        if (lines.isEmpty()) {
            qDebug() << "[HTTP-LOG] 错误：请求数据为空";
            sendResponse(socket, 400, "Bad Request", QJsonObject());
            return;
        }

        QStringList requestLine = lines[0].split(" ");
        if (requestLine.size() != 3) {
            qDebug() << "[HTTP-LOG] 错误：请求行格式非法 ->" << lines[0];
            sendResponse(socket, 400, "Bad Request", QJsonObject());
            return;
        }

        QString method = requestLine[0];
        QString path = requestLine[1];
        QString version = requestLine[2];

        QUrlQuery query;
        int queryIndex = path.indexOf('?');
        QString pathWithoutQuery = path;
        if (queryIndex != -1) {
            query = QUrlQuery(path.mid(queryIndex + 1));
            pathWithoutQuery = path.left(queryIndex);
        }

        QJsonObject requestBody;
        int bodyIndex = request.indexOf("\r\n\r\n");
        if (bodyIndex != -1 && (method == "POST" || method == "PUT" || method == "DELETE")) {
            QByteArray bodyData = data.mid(bodyIndex + 4);
            QJsonDocument doc = QJsonDocument::fromJson(bodyData);
            if (!doc.isNull() && doc.isObject()) {
                requestBody = doc.object();
            }
        }

        qDebug() << "-------------------------------------";
        qDebug() << "【收到前端请求】" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        qDebug() << "请求方式：" << method;
        qDebug() << "请求接口：" << pathWithoutQuery;
        qDebug() << "POST请求体：" << requestBody;

        QString authorizationHeader = "";
        for (const QString& line : lines) {
            if (line.startsWith("Authorization:", Qt::CaseInsensitive)) {
                authorizationHeader = line.section(':', 1).trimmed();
                qDebug() << "[HTTP-LOG] 提取到 Authorization Header:" << authorizationHeader;
                break;
            }
        }

        // ================ [修复：清洗双重 Bearer 前缀] ================
        while (authorizationHeader.startsWith("Bearer ", Qt::CaseInsensitive)) {
            authorizationHeader = authorizationHeader.mid(7).trimmed();
        }
        // ===========================================================

        QJsonObject responseObj;
        int statusCode = 200;
        QString statusText = "OK";

        // ========== 1. 用户主要功能模块 ==========

        if (method == "POST" && pathWithoutQuery == "/api/user/login") {
            qDebug() << "[HTTP-LOG] 匹配路由: 用户登录";
            QString telephone = requestBody["telephone"].toString();
            QString password = requestBody["password"].toString();
            UserDao dao;
            responseObj = dao.login(telephone, password);
        }
        else if (method == "POST" && pathWithoutQuery == "/api/user/register") {
            qDebug() << "[HTTP-LOG] 匹配路由: 用户注册";
            QString telephone = requestBody["telephone"].toString();
            QString password = requestBody["password"].toString();
            QString username = requestBody["username"].toString();
            UserDao dao;
            responseObj = dao.registerUser(telephone, password, username);
        }
        else if (method == "GET" && pathWithoutQuery == "/api/flight/search") {
            qDebug() << "[HTTP-LOG] 匹配路由: 航班查询";
            QString departureCity = query.queryItemValue("departureCity");
            QString arrivalCity = query.queryItemValue("arrivalCity");
            QDate date = QDate::fromString(query.queryItemValue("date"), "yyyy-MM-dd");
            QString airlineCompany = query.queryItemValue("airlineCompany");

            bool minPriceOk;
            int minPrice = query.queryItemValue("minPrice").toInt(&minPriceOk);
            if (!minPriceOk) minPrice = -1;

            bool maxPriceOk;
            int maxPrice = query.queryItemValue("maxPrice").toInt(&maxPriceOk);
            if (!maxPriceOk) maxPrice = -1;

            bool pageNumOk;
            int pageNum = query.queryItemValue("pageNum").toInt(&pageNumOk);
            if (!pageNumOk) pageNum = 1;

            bool pageSizeOk;
            int pageSize = query.queryItemValue("pageSize").toInt(&pageSizeOk);
            if (!pageSizeOk) pageSize = 10;

            FlightDao dao;
            QList<Flight*> flights = dao.queryFlightsWithFilters(departureCity, arrivalCity, date, airlineCompany, minPrice, maxPrice, pageNum, pageSize);
            int totalCount = dao.getFlightCountWithFilters(departureCity, arrivalCity, date, airlineCompany, minPrice, maxPrice);
            responseObj = dao.flightsToApiJson(flights, totalCount, pageNum, pageSize);
            qDeleteAll(flights);
        }
        else if (method == "POST" && pathWithoutQuery == "/api/order/book") {
            qDebug() << "[HTTP-LOG] 匹配路由: 机票预订";
            int flightId = requestBody["flightId"].toInt();
            QJsonArray passengersArray = requestBody["passengers"].toArray();
            QList<QPair<QString, QString>> passengers;
            for (const QJsonValue& val : passengersArray) {
                QJsonObject pObj = val.toObject();
                passengers.append({pObj["name"].toString(), pObj["idCard"].toString()});
            }
            OrderInfoDao dao;
            responseObj = dao.bookOrder(authorizationHeader, flightId, passengers);
        }
        else if (method == "POST" && pathWithoutQuery == "/api/order/pay") {
            qDebug() << "[HTTP-LOG] 匹配路由: 订单支付";
            QString orderId = requestBody["orderId"].toString();
            OrderInfoDao dao;
            responseObj = dao.payOrder(authorizationHeader, orderId);
        }
        else if (method == "GET" && pathWithoutQuery == "/api/order/query") {
            qDebug() << "[HTTP-LOG] 匹配路由: 个人订单查询";
            QString status = query.queryItemValue("status");
            OrderInfoDao dao;
            responseObj = dao.queryOrderByUserId(authorizationHeader, status);
        }
        else if (method == "POST" && pathWithoutQuery == "/api/order/change") {
            qDebug() << "[HTTP-LOG] 匹配路由: 订单改签 (POST)";

            // 1. 验证Token
            QString token = authorizationHeader;
            int userId = TokenTool::verifyToken(token);

            if (userId == -1) {
                qDebug() << "[HTTP-LOG] Token验证失败";
                responseObj["code"] = 401;
                responseObj["message"] = "Token无效或已过期";
                responseObj["data"] = QJsonValue::Null;
            } else {
                // 2. 解析参数
                QString orderId = requestBody["orderId"].toString();
                int newFlightId = requestBody["newFlightId"].toInt();

                if (orderId.isEmpty() || newFlightId <= 0) {
                    responseObj["code"] = 400;
                    responseObj["message"] = "参数错误: orderId和newFlightId不能为空";
                    responseObj["data"] = QJsonValue::Null;
                } else {
                    qDebug() << "[HTTP-LOG] 改签请求 - userId:" << userId
                             << "orderId:" << orderId
                             << "newFlightId:" << newFlightId;

                    // 3. 直接调用OrderChangeDao
                    OrderChangeDao dao;
                    responseObj = dao.changeOrder(orderId, newFlightId);

                    qDebug() << "[HTTP-LOG] 改签响应:" << QJsonDocument(responseObj).toJson();
                }
            }
        }
        else if ((method == "PUT" || method == "POST") && pathWithoutQuery == "/api/order/refund") {
            qDebug() << "[HTTP-LOG] 匹配路由: 订单退签";
            QString orderId = requestBody["orderId"].toString();
            OrderInfoDao dao;
            responseObj = dao.refundOrder(authorizationHeader, orderId);
        }
        else if ((method == "DELETE" || method == "POST") && pathWithoutQuery == "/api/order/delete") {
            qDebug() << "[HTTP-LOG] 匹配路由: 删除订单";
            QString orderId = requestBody["orderId"].toString();
            if (orderId.isEmpty()) {
                responseObj["code"] = 400;
                responseObj["message"] = "缺失订单ID参数";
            } else {
                OrderInfoDao dao;
                responseObj = dao.deleteOrder(authorizationHeader, orderId);
            }
        }
        else if (method == "GET" && pathWithoutQuery == "/api/user/info/get") {
            qDebug() << "[HTTP-LOG] 匹配路由: 获取用户信息";
            UserDao dao;
            responseObj = dao.getUserInfoByToken(authorizationHeader);
        }
        else if ((method == "PUT" || method == "POST") && pathWithoutQuery == "/api/user/info/update") {
            qDebug() << "[HTTP-LOG] 匹配路由: 修改用户信息/充值";
            QString newUsername = requestBody["username"].toString();
            QString oldPassword = requestBody["oldPassword"].toString();
            QString newPassword = requestBody["password"].toString();
            int balance = requestBody.contains("balance") ? requestBody["balance"].toInt() : -1;
            UserDao dao;
            responseObj = dao.updateUserInfoApi(authorizationHeader, newUsername, oldPassword, newPassword, balance);
        }

        // ========== 2. 管理员主要功能模块 ==========

        else if (method == "POST" && pathWithoutQuery == "/api/admin/login") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员登录";
            QString account = requestBody["account"].toString();
            QString pwd = requestBody["password"].toString();
            responseObj = AdminService::login(account, pwd);
        }
        else if (method == "GET" && pathWithoutQuery == "/api/admin/info") {
            qDebug() << "[HTTP-LOG] 匹配路由: 获取管理员信息";
            QNetworkRequest req;
            req.setRawHeader("Authorization", authorizationHeader.toUtf8());
            responseObj = AdminService::getAdminInfo(req);
        }
        else if (method == "POST" && pathWithoutQuery == "/api/admin/flight/add") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员添加航班";
            int adminId = TokenTool::verifyToken(authorizationHeader);
            if (adminId == -1) {
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效或已过期";
            } else {
                responseObj = AdminService::addFlight(requestBody);
            }
        }
        else if (method == "POST" && pathWithoutQuery == "/api/admin/flight/delete") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员删除航班";
            int adminId = TokenTool::verifyToken(authorizationHeader);
            if (adminId == -1) {
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效或已过期";
            } else {
                int flightId = requestBody["flightId"].toInt();
                responseObj = AdminService::deleteFlight(flightId);
            }
        }
        else if (method == "POST" && pathWithoutQuery == "/api/admin/flight/update") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员更新航班";
            int adminId = TokenTool::verifyToken(authorizationHeader);
            if (adminId == -1) {
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效或已过期";
            } else {
                responseObj = AdminService::updateFlight(requestBody);
            }
        }
        else if (method == "GET" && pathWithoutQuery == "/api/admin/flight/search") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员查询航班";
            int adminId = TokenTool::verifyToken(authorizationHeader);
            if (adminId == -1) {
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效或已过期";
            } else {
                QJsonObject params;
                params["flightNumber"] = query.queryItemValue("flightNumber");
                params["departureCity"] = query.queryItemValue("departureCity");
                params["arrivalCity"] = query.queryItemValue("arrivalCity");
                params["date"] = query.queryItemValue("date");
                bool pNOk, pSOk;
                params["pageNum"] = query.queryItemValue("pageNum").toInt(&pNOk) ? query.queryItemValue("pageNum").toInt() : 1;
                params["pageSize"] = query.queryItemValue("pageSize").toInt(&pSOk) ? query.queryItemValue("pageSize").toInt() : 20;

                FlightDao dao;
                QList<Flight*> flights = dao.queryFlightsWithFilters(params["departureCity"].toString(), params["arrivalCity"].toString(), QDate::fromString(params["date"].toString(), "yyyy-MM-dd"), "", -1, -1, params["pageNum"].toInt(), params["pageSize"].toInt());
                int totalCount = dao.getFlightCountWithFilters(params["departureCity"].toString(), params["arrivalCity"].toString(), QDate::fromString(params["date"].toString(), "yyyy-MM-dd"), "", -1, -1);
                responseObj = dao.flightsToApiJson(flights, totalCount, params["pageNum"].toInt(), params["pageSize"].toInt());
                qDeleteAll(flights);
            }
        }
        else if (method == "GET" && pathWithoutQuery == "/api/admin/user/all") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员查看所有客户";

            // 1. 处理 Token
            QString pureToken = authorizationHeader;
            while (pureToken.startsWith("Bearer ", Qt::CaseInsensitive)) {
                pureToken = pureToken.mid(7).trimmed();
            }

            int adminId = TokenTool::verifyToken(pureToken);

            if (adminId == -1) {
                qDebug() << "[HTTP-LOG] 校验失败: 管理员Token无效 ->" << pureToken;
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效";
                responseObj["data"] = QJsonValue::Null;
            } else {
                // 2. 准备查询参数 (修正：增加 userId 的提取)
                QJsonObject params;

                // --- 新增：从 URL Query 提取 userId ---
                if (query.hasQueryItem("userId")) {
                    params["userId"] = query.queryItemValue("userId");
                }

                params["telephone"] = query.queryItemValue("telephone");
                params["username"] = query.queryItemValue("username");

                // 将 Query 里的字符串转为数字
                params["pageNum"] = query.queryItemValue("pageNum").isEmpty() ? 1 : query.queryItemValue("pageNum").toInt();
                params["pageSize"] = query.queryItemValue("pageSize").isEmpty() ? 20 : query.queryItemValue("pageSize").toInt();

                // 3. 调用 Service
                responseObj = AdminService::queryAllUsers(params);
            }
        }
        else if (method == "GET" && pathWithoutQuery == "/api/admin/order/all") {
            qDebug() << "[HTTP-LOG] 匹配路由: 管理员查看所有订单";
            if (TokenTool::verifyToken(authorizationHeader) == -1) {
                responseObj["code"] = 500;
                responseObj["message"] = "管理员Token无效";
            } else {
                QJsonObject params;
                params["orderId"] = query.queryItemValue("orderId");
                params["status"] = query.queryItemValue("status");

                // ===== 确保提取telephone参数 =====
                QString telephone = query.queryItemValue("telephone");
                qDebug() << "[HTTP-LOG] 从URL提取的电话参数: " << telephone;
                params["telephone"] = telephone;
                // =================================

                params["pageNum"] = query.queryItemValue("pageNum").toInt() > 0 ?
                                        query.queryItemValue("pageNum").toInt() : 1;
                params["pageSize"] = query.queryItemValue("pageSize").toInt() > 0 ?
                                         query.queryItemValue("pageSize").toInt() : 20;

                qDebug() << "[HTTP-LOG] 传递给Service的完整参数: " << params;

                responseObj = AdminService::queryAllOrders(params);
            }
        }
        else {
            qDebug() << "[HTTP-LOG] 未匹配路由:" << pathWithoutQuery;
            statusCode = 404;
            statusText = "Not Found";
            responseObj["code"] = 404;
            responseObj["message"] = "接口不存在";
        }

        qDebug() << "[HTTP-LOG] 即将发送响应, Code:" << statusCode << "Body:" << QJsonDocument(responseObj).toJson(QJsonDocument::Compact);
        sendResponse(socket, statusCode, statusText, responseObj);
    }

private:
    void sendResponse(QTcpSocket *socket, int statusCode, const QString &statusText, const QJsonObject &responseObj)
    {
        QJsonDocument doc(responseObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        QString response = QString("HTTP/1.1 %1 %2\r\n").arg(statusCode).arg(statusText);
        response += "Content-Type: application/json; charset=utf-8\r\n";
        response += QString("Content-Length: %1\r\n").arg(jsonData.size());
        response += "Connection: close\r\n\r\n";
        response += QString::fromUtf8(jsonData);

        socket->write(response.toUtf8());
        socket->flush();
        socket->disconnectFromHost();
        qDebug() << "[HTTP-LOG] <<< 响应已发送，连接关闭\n";
    }
};

#endif // HTTPSERVER_H
