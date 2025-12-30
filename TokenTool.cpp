#include "TokenTool.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMutexLocker>
#include <QDebug>

// 初始化静态成员变量
QMap<QString, QPair<int, qint64>> TokenTool::m_tokenMap;
QMutex TokenTool::m_mutex;

// 生成带过期时间的Token
QString TokenTool::generateToken(int userId, int expireSeconds) {
    // 生成随机数增强安全性
    qint64 nonce = QRandomGenerator::global()->generate64();
    // 当前时间戳（秒）
    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    // 过期时间戳（秒）
    qint64 expireTime = currentTime + expireSeconds;

    // 拼接原始字符串（包含用户ID、时间、随机数、密钥）
    QString baseStr = QString("%1_%2_%3_%4_%5")
                          .arg(userId)
                          .arg(currentTime)
                          .arg(expireTime)
                          .arg(nonce)
                          .arg(getSecretKey());

    // SHA256哈希生成Token主体
    QByteArray hashData = QCryptographicHash::hash(baseStr.toUtf8(), QCryptographicHash::Sha256);
    QString tokenBody = hashData.toHex();

    // Token格式：哈希值_过期时间（方便后续解析）
    return tokenBody + "_" + QString::number(expireTime);
}

// 保存Token映射关系
void TokenTool::saveToken(int userId, const QString& token) {
    QMutexLocker locker(&m_mutex);

    // 注意：这里传入的token应该是generateToken生成的原始token（不含Bearer前缀）
    QString pureToken = token.trimmed();

    // 分割Token解析过期时间
    QStringList tokenParts = pureToken.split("_");
    if (tokenParts.size() != 2) {
        qDebug() << "保存Token失败：Token格式不正确";
        return;
    }

    bool ok;
    qint64 expireTime = tokenParts[1].toLongLong(&ok);
    if (!ok || expireTime <= QDateTime::currentSecsSinceEpoch()) {
        qDebug() << "保存Token失败：Token已过期或时间解析失败";
        return;
    }

    // 存储纯净Token
    m_tokenMap[pureToken] = QPair<int, qint64>(userId, expireTime);
    qDebug() << "保存Token成功：userId =" << userId << "，过期时间=" << expireTime;
}

// 核心：处理Bearer前缀的工具函数
QString TokenTool::removeBearerPrefix(const QString& token) {
    QString pureToken = token.trimmed();
    // 兼容大小写+多空格场景
    if (pureToken.startsWith("Bearer ", Qt::CaseInsensitive)) {
        pureToken = pureToken.mid(7).trimmed();
    }
    return pureToken;
}

int TokenTool::getUserIdByToken(const QString& token) {
    QMutexLocker locker(&m_mutex);

    // 1. 处理Bearer前缀
    QString pureToken = removeBearerPrefix(token);

    // 2. 空值校验
    if (pureToken.isEmpty()) {
        qDebug() << "Token验证失败：Token为空";
        return -1;
    }

    // 3. 检查Token格式是否正确
    QStringList parts = pureToken.split("_");
    if (parts.size() != 2) {
        qDebug() << "Token验证失败：Token格式不正确";
        return -1;
    }

    // 4. 检查Token是否存在
    if (!m_tokenMap.contains(pureToken)) {
        qDebug() << "Token验证失败：Token不存在或已失效";
        return -1;
    }

    // 5. 解析用户ID和过期时间
    QPair<int, qint64> userInfo = m_tokenMap[pureToken];
    int userId = userInfo.first;
    qint64 expireTime = userInfo.second;

    // 6. 检查Token是否过期
    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    if (currentTime > expireTime) {
        qDebug() << "Token验证失败：Token已过期";
        m_tokenMap.remove(pureToken);
        return -1;
    }

    return userId;
}

// 注销Token
void TokenTool::removeToken(const QString& token) {
    QMutexLocker locker(&m_mutex);
    QString pureToken = removeBearerPrefix(token);
    if (!pureToken.isEmpty()) {
        m_tokenMap.remove(pureToken);
        qDebug() << "Token已注销：" << pureToken;
    }
}

// 获取加密密钥
QString TokenTool::getSecretKey() {
    return "flight_system_2025_secret_key_123456";
}

// 从请求头提取Bearer token
QString TokenTool::getTokenFromHeader(const QByteArray& authorizationHeader) {
    QString headerStr = QString::fromUtf8(authorizationHeader);
    return removeBearerPrefix(headerStr);
}

// 验证token并返回用户ID
int TokenTool::verifyToken(const QString& token) {
    // 直接调用getUserIdByToken，它内部会处理Bearer前缀
    return getUserIdByToken(token);
}

// 调试函数：打印当前Token状态
void TokenTool::debugTokenStatus() {
    QMutexLocker locker(&m_mutex);
    qDebug() << "=== Token状态调试信息 ===";
    qDebug() << "当前存储的Token数量：" << m_tokenMap.size();

    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    qDebug() << "当前时间戳：" << currentTime;

    int validCount = 0;
    for (auto it = m_tokenMap.begin(); it != m_tokenMap.end(); ++it) {
        bool expired = currentTime > it.value().second;
        qDebug() << "Token:" << it.key()
                 << "，userId:" << it.value().first
                 << "，过期时间:" << it.value().second
                 << "，状态:" << (expired ? "已过期" : "有效");
        if (!expired) validCount++;
    }
    qDebug() << "有效Token数量：" << validCount;
    qDebug() << "=========================";
}
