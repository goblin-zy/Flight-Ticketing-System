#include "UserContext.h"

// 构造函数：初始化并连接信号槽
UserContext::UserContext(NetworkManager* manager, QObject *parent)
    : QObject(parent), m_manager(manager), m_myMoney(0), m_isAdmin(false)
{
    // 连接 NetworkManager 的信号到自己的处理函数
    // 注意：在复杂项目中，通常会通过 requestid 来区分请求，
    // 这里简化处理：通过返回数据的结构特征来判断是否是用户信息包
    connect(m_manager, &NetworkManager::requestSuccess, this, &UserContext::onApiSuccess);
    connect(m_manager, &NetworkManager::requestFailed, this, &UserContext::onApiFailed);
}



// --- 核心功能：主动更新用户信息 ---  //token改变时会自动调用
void UserContext::updateUserInfo() {
    if (m_myToken.isEmpty()) {
        qWarning() << "UserContext: Token 为空，无法更新用户信息";
        return;
    }

    // 根据m_isAdmin属性决定请求路径
    QString url;
    if (m_isAdmin) {
        // 管理员请求地址
        url = "/api/admin/info";
        qDebug() << "UserContext: 开始主动更新管理员信息...";
    } else {
        // 普通用户请求地址
        url = "/api/user/info/get";
        qDebug() << "UserContext: 开始主动更新用户信息...";
    }

    // 请求细节：使用GET方法，并携带当前Token
    m_manager->request(url, NetworkManager::GET, QJsonObject(), m_myToken);
}



// --- 处理 API 成功响应 ---
void UserContext::onApiSuccess(const QJsonObject &root) {
    // 1. 将 response 转换为格式化的 JSON 字符串并输出
    QJsonDocument doc(root);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    qDebug() << "Received response:" << jsonString;

    // 2. 检查 API 状态码
    if (root.contains("code") && root["code"].toInt() != 200) {
        // 如果不是 200，说明请求虽然通了但业务失败，输出错误信息
        if (root.contains("message")) {
            qWarning() << "更新用户信息失败:" << root["message"].toString();
        } else {
            qWarning() << "更新用户信息失败: 未知错误";
        }
        return;
    }

    // 3. 状态洗涤（State Cleansing）：重置所有身份和业务数据属性为默认值
    // 防止跨角色登录时数据相互污染
    //qDebug() << "UserContext: 开始状态洗涤，重置所有身份和业务数据属性...";
    //setUserId("");           // userId -> ""
    //setUserName("");         // userName -> ""
    //setMyMoney(0);            // myMoney -> 0
    //setMyTelephone("");      // myTelephone -> ""
    //setAdminAccount("");     // adminAccount -> ""
    //setIsAdmin(false);        // isAdmin -> false
    // 注意：不重置myToken，因为当前函数正在处理该Token对应的有效响应
    //状态洗涤导致不能回到个人中心。

    // 4. 检查 data 字段是否存在
    if (!root.contains("data")) return;

    QJsonObject data = root["data"].toObject();
    
    // 5. 根据返回数据特征判断是管理员还是普通用户
    if (data.contains("adminId") || data.contains("adminAccount")) {
        // 管理员逻辑分支
        qDebug() << "UserContext: 收到管理员信息响应，开始同步数据...";
        
        // 设置为管理员
        setIsAdmin(true);
        
        // 管理员ID: API是Int, C++是String, 需要转换
        if (data.contains("adminId")) {
            qDebug() << "更新管理员ID";
            setUserId(QString::number(data["adminId"].toInt()));
        }
        
        // 管理员用户名
        if (data.contains("adminName")) {
            qDebug() << "更新管理员用户名";
            setUserName(data["adminName"].toString());
        }
        
        // 管理员账号（对应用户端的telephone）
        if (data.contains("adminAccount")) {
            qDebug() << "更新管理员账号";
            setAdminAccount(data["adminAccount"].toString());
            // 同时设置到myTelephone，保持兼容性
            setMyTelephone(data["adminAccount"].toString());
        }
        
        // 管理员没有余额属性，设置为0
        setMyMoney(0);
        
        qDebug() << "已成功更新管理员信息";
    }


    else if (data.contains("username") || data.contains("balance")) {
        // 普通用户逻辑分支（原有逻辑）
        qDebug() << "UserContext: 收到用户信息响应，开始同步数据...";
        
        // 设置为普通用户
        setIsAdmin(false);
        
        // ID: API是Int, C++是String, 需要转换
        if (data.contains("id")) {
            qDebug() << "更新用户ID";
            setUserId(QString::number(data["id"].toInt()));
        }

        // Username
        if (data.contains("username")) {
            qDebug() << "更新用户名";
            setUserName(data["username"].toString());
        }

        // Telephone
        if (data.contains("telephone")) {
            qDebug() << "更新用户电话";
            setMyTelephone(data["telephone"].toString());
        }

        // Balance (Money): 文档说明单位是分(Int)
        if (data.contains("balance")) {
            qDebug() << "更新用户余额";
            setMyMoney(data["balance"].toInt());
        }

        // Avatar: 补全头像
 /*   if (data.contains("avatar")) {
        qDebug() << "更新用户头像";
        setMyAvatar(data["avatar"].toString());
    }*/

        qDebug() << "已成功更新用户信息";
    }
    
    // 在 onApiSuccess 函数的最后一行（大括号前）添加：
    qDebug() << "数据同步完成，发送就绪信号...";
    emit userInfoReady();
}

// --- 处理 API 失败响应 ---
void UserContext::onApiFailed(const QString &error) {
    qWarning() << "Failed to update user info:" << error;
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
/*QString UserContext::myAvatar() const { return m_myAvatar; }
void UserContext::setMyAvatar(const QString &avatar) {
    if (m_myAvatar != avatar) {
        m_myAvatar = avatar;
        emit myAvatarChanged();
    }
}*/

// --- 管理员相关 Getters & Setters 实现 ---
bool UserContext::isAdmin() const { return m_isAdmin; }
void UserContext::setIsAdmin(bool isAdmin) {
    if (m_isAdmin != isAdmin) {
        m_isAdmin = isAdmin;
        emit isAdminChanged();
    }
}

QString UserContext::adminAccount() const { return m_adminAccount; }
void UserContext::setAdminAccount(const QString &adminAccount) {
    if (m_adminAccount != adminAccount) {
        m_adminAccount = adminAccount;
        emit adminAccountChanged();
    }
}
