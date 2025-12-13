#include "UserContext.h"

// 构造函数：初始化并连接信号槽
UserContext::UserContext(NetworkManager* manager, QObject *parent)
    : QObject(parent), m_manager(manager), m_myMoney(0)
{
    // 连接 NetworkManager 的成功信号到自己的处理函数
    // 注意：在复杂项目中，通常会通过 requestid 来区分请求，
    // 这里简化处理：通过返回数据的结构特征来判断是否是用户信息包
    connect(m_manager, &NetworkManager::requestSuccess, this, &UserContext::onApiSuccess);
}

// --- 核心功能：主动更新用户信息 ---
void UserContext::updateUserInfo() {
    if (m_myToken.isEmpty()) {
        qWarning() << "UserContext: Token 为空，无法更新用户信息";
        return;
    }

    // 根据 API 文档 1.4.1
    // URL: /api/user/info
    // Method: GET
    // Header: Authorization (在 NetworkManager 内部通过 Token 参数处理)
    qDebug() << "UserContext: 开始主动更新用户信息...";
    m_manager->request("/api/user/info", NetworkManager::GET, QJsonObject(), m_myToken);
}

// --- 处理 API 响应 ---
void UserContext::onApiSuccess(const QJsonObject &root) {
    // 1. 检查 API 状态码
    if (root.contains("code") && root["code"].toInt() != 200) {
        // 如果不是 200，说明请求虽然通了但业务失败，忽略
        return;
    }

    // 2. 简单的特征识别：防止混淆其他请求的响应
    // 只有当 data 字段存在，且 data 里包含 "balance" 或 "username" 时，才认为是用户信息包
    if (!root.contains("data")) return;

    QJsonObject data = root["data"].toObject();
    if (!data.contains("username") && !data.contains("balance")) {
        return;
    }

    qDebug() << "UserContext: 收到用户信息响应，开始同步数据...";

    // 3. 映射数据 (根据新的api文档.md)

    // ID: API是Int, C++是String, 需要转换
    if (data.contains("id")) {
        setUserId(QString::number(data["id"].toInt()));
    }

    // Username
    if (data.contains("username")) {
        setUserName(data["username"].toString());
    }

    // Telephone
    if (data.contains("telephone")) {
        setMyTelephone(data["telephone"].toString());
    }

    // Balance (Money): 文档说明单位是分(Int)
    if (data.contains("balance")) {
        setMyMoney(data["balance"].toInt());
    }

    // Avatar: 补全头像
    if (data.contains("avatar")) {
        setMyAvatar(data["avatar"].toString());
    }

    // Email: 按照你的要求，这里不进行处理
}

// --- Getters & Setters 实现 ---

QString UserContext::userId() const { return m_userId; }
void UserContext::setUserId(const QString &userId) {
    if (m_userId != userId) {
        m_userId = userId;
        emit userIdChanged();
    }
}

QString UserContext::userName() const { return m_userName; }
void UserContext::setUserName(const QString &userName) {
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}

QString UserContext::myToken() const { return m_myToken; }
void UserContext::setMyToken(const QString &myToken) {
    if (m_myToken != myToken) {
        m_myToken = myToken;
        emit myTokenChanged();

        // 当 Token 变化且不为空时，可以自动触发一次更新
        if(!myToken.isEmpty()) {
            updateUserInfo();
        }
    }
}

int UserContext::myMoney() const { return m_myMoney; }
void UserContext::setMyMoney(int myMoney) {
    if (m_myMoney != myMoney) {
        m_myMoney = myMoney;
        emit myMoneyChanged();
    }
}

QString UserContext::myTelephone() const { return m_myTelephone; }
void UserContext::setMyTelephone(const QString &myTelephone) {
    if (m_myTelephone != myTelephone) {
        m_myTelephone = myTelephone;
        emit myTelephoneChanged();
    }
}

// Avatar 实现
QString UserContext::myAvatar() const { return m_myAvatar; }
void UserContext::setMyAvatar(const QString &avatar) {
    if (m_myAvatar != avatar) {
        m_myAvatar = avatar;
        emit myAvatarChanged();
    }
}
