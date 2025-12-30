#ifndef TOKENTOOL_H
#define TOKENTOOL_H

#include <QString>
#include <QMap>
#include <QPair>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMutex>

class TokenTool {
public:
    // 生成Token（默认2小时过期，可自定义过期秒数）
    static QString generateToken(int userId, int expireSeconds = 7200);

    // 保存Token与用户ID、过期时间的映射
    static void saveToken(int userId, const QString& token);

    // 通过Token获取用户ID（-1表示Token无效/过期）
    // 注意：此函数内部会自动处理Bearer前缀，无需外部手动处理
    static int getUserIdByToken(const QString& token);

    // 注销Token（用户登出时调用）
    static void removeToken(const QString& token);

    // 工具函数：移除Bearer前缀（仅用于特殊场景，一般情况使用getUserIdByToken或verifyToken）
    static QString removeBearerPrefix(const QString& token);

    // 从请求头提取Bearer token（推荐使用此函数或直接调用getUserIdByToken）
    static QString getTokenFromHeader(const QByteArray& authorizationHeader);

    // 验证token并返回用户ID（无效返回-1）
    // 注意：此函数内部会自动处理Bearer前缀，无需外部手动处理
    static int verifyToken(const QString& token);

    // 调试函数：打印当前Token状态
    static void debugTokenStatus();

private:
    // 从配置文件/常量获取密钥（可自行修改密钥内容）
    static QString getSecretKey();

    // 存储结构：Token -> (UserID, 过期时间戳)
    static QMap<QString, QPair<int, qint64>> m_tokenMap;
    // 线程锁（保证多线程安全）
    static QMutex m_mutex;
};

#endif // TOKENTOOL_H
