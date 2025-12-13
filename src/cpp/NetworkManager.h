#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    // 保持原有的请求方式枚举
    enum RequestMethod {
        GET,
        POST,
        PUT,
        DELETE
    };
    Q_ENUM(RequestMethod)

    // 构造函数改名
    explicit NetworkManager(QObject *parent = nullptr, QString api_url = "http://localhost:8080");

    // 核心请求函数，保持 Q_INVOKABLE 以便 QML 调用
    Q_INVOKABLE void request(
        const QString &url,
        RequestMethod method,
        const QJsonObject &data = QJsonObject(),
        const QString Token = ""
        );

signals:
    void requestSuccess(const QJsonObject &response);
    void requestFailed(const QString &errorMessage);

private:
    QNetworkAccessManager *m_manager; // 改名为更通用的 manager
    QString m_apiUrl;                 // 统一变量命名风格

    QUrl buildUrlWithParams(const QString &baseUrl, const QJsonObject &params);
};

#endif // NETWORKMANAGER_H
