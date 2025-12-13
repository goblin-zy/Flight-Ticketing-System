#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject *parent, QString api_url)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)), m_apiUrl(api_url) {
}

void NetworkManager::request(const QString &url, RequestMethod method, const QJsonObject &data, const QString Token) {
    QNetworkRequest request;
    QNetworkReply *reply = nullptr;

    QString finalUrl = url;
    // 处理相对路径
    if(url.startsWith("/")){
        finalUrl = m_apiUrl + url;
    }

    // 设置 Token
    if (!Token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(Token).toUtf8());
    }

    // 根据不同方法构建请求
    switch (method) {
    case GET: {
        QUrl fullUrl = buildUrlWithParams(finalUrl, data);
        request.setUrl(fullUrl);
        reply = m_manager->get(request);
        break;
    }
    case POST: {
        request.setUrl(finalUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = m_manager->post(request, QJsonDocument(data).toJson());
        break;
    }
    case PUT: {
        request.setUrl(finalUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = m_manager->put(request, QJsonDocument(data).toJson());
        break;
    }
    case DELETE: {
        request.setUrl(finalUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = m_manager->deleteResource(request);
        break;
    }
    }

    // 连接信号槽处理响应
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit requestSuccess(doc.object());
        } else {
            emit requestFailed(reply->errorString());
        }
        reply->deleteLater();
    });
}

QUrl NetworkManager::buildUrlWithParams(const QString &baseUrl, const QJsonObject &params) {
    QUrl url(baseUrl);
    QUrlQuery query;

    for (auto it = params.begin(); it != params.end(); ++it) {
        query.addQueryItem(it.key(), it.value().toString());
    }

    url.setQuery(query);
    return url;
}
